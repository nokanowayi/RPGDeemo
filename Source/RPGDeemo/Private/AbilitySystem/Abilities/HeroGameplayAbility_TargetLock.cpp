// MyRPGDemo


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"

#include "Characters/WarriorHeroCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	if (!TryLockOnTarget())
	{
		return;
	}

	InitTargetLockMovement();
	
	if (IsLocallyControlled())
	{
		InitTargetLockMappingContext();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CleanUp();
	ResetTargetMovement();
	ResetTargetMappingContext();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!IsValid(CurrentLockActor) || UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockActor,WarriorGameplayTags::Shared_Status_Dead)||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacter(),WarriorGameplayTags::Shared_Status_Dead))
	{
		CancelTargetLockAbility();
		return;
	}

	// 控件位置与控制器旋转只对本地玩家有意义
	if (!IsLocallyControlled())
	{
		return;
	}

	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotation = !UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacter(),WarriorGameplayTags::Player_Status_Rolling)
	&&
	!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacter(),WarriorGameplayTags::Player_Status_Blocking);

	if (bShouldOverrideRotation)
	{
		FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(GetHeroCharacter()->GetActorLocation(),CurrentLockActor->GetActorLocation());

		Rotator -= FRotator(TargetLockCameraDis,0,0);
		FRotator ControllerRotator = GetHeroController()->GetControlRotation();

		FRotator TargetRotator = FMath::RInterpTo(ControllerRotator,Rotator,DeltaTime,TargetLockCameraSpeed);

		GetHeroController()->SetControlRotation(FRotator(TargetRotator.Pitch,TargetRotator.Yaw,ControllerRotator.Roll));
		GetHeroCharacter()->SetActorRotation(FRotator(0.f,TargetRotator.Yaw,0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& SwitchDirectionTag)
{
	GetAvailableActorsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewLockActor = nullptr;

	GetAvailableActorsAroundTaget(ActorsOnLeft,ActorsOnRight);

	if (SwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewLockActor = GetClosestActorToLock(ActorsOnLeft);
	}
	else if (SwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Right)
	{
		NewLockActor = GetClosestActorToLock(ActorsOnRight);
	}

	if (NewLockActor)
	{
		CurrentLockActor = NewLockActor;
		SetTargetLockWidgetPosition();
	}
}

bool UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return false;
	}

	CurrentLockActor = GetClosestActorToLock();

	if (!CurrentLockActor)
	{
		CancelTargetLockAbility();
		return false;
	}

	// 服务端上非本地PC没有 LocalPlayer，AddToViewport会读到空指针
	if (IsLocallyControlled())
	{
		DrawTargetWidget();
		SetTargetLockWidgetPosition();
	}

	return true;
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();
	TArray<FHitResult> BoxTraceHits;
	
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacter(),
		GetHeroCharacter()->GetActorLocation(),
		GetHeroCharacter()->GetActorLocation() + GetAvatarActorFromActorInfo()->GetActorForwardVector() * TraceDistance,
		TraceBoxSize/2,
		GetHeroCharacter()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape?EDrawDebugTrace::Persistent:EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor == GetHeroCharacter() || HitActor->IsA<AWarriorHeroCharacter>())
			{
				continue;
			}
			
			if (Cast<APawn>(HitActor))
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTaget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (AvailableActorsToLock.IsEmpty() || !CurrentLockActor)
	{
		CancelTargetLockAbility();
		return;
	}
	FVector PlayerLocation = GetHeroCharacter()->GetActorLocation();
	FVector PlayerToCurrentLocation = (CurrentLockActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	for (AActor* Actor : AvailableActorsToLock)
	{
		if (Actor != CurrentLockActor)
		{
			FVector PlayerToAvailableNormal = (Actor->GetActorLocation() - PlayerLocation).GetSafeNormal();
			FVector CrossRes = FVector::CrossProduct(PlayerToCurrentLocation, PlayerToAvailableNormal);
			if (CrossRes.Z > 0.f)
			{
				OutActorsOnRight.AddUnique(Actor);
			}
			else
			{
				OutActorsOnLeft.AddUnique(Actor);
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetClosestActorToLock()
{
	float Dis = 0.f;
	return  UGameplayStatics::FindNearestActor(GetHeroCharacter()->GetActorLocation(),AvailableActorsToLock,Dis);
}

AActor* UHeroGameplayAbility_TargetLock::GetClosestActorToLock(TArray<AActor*> ActorsToCheck)
{
	float Dis = 0.f;
	return  UGameplayStatics::FindNearestActor(GetHeroCharacter()->GetActorLocation(),ActorsToCheck,Dis);
}

void UHeroGameplayAbility_TargetLock::DrawTargetWidget()
{
	if (DrawTargetLockWidget)
	{
		return;
	}

	if (!TargetLockWidgetClass)
	{
		Debug::Print(TEXT("TargetLockWidgetClass 未设置，跳过锁定控件创建"));
		return;
	}

	DrawTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroController(),TargetLockWidgetClass);

	if (DrawTargetLockWidget)
	{
		DrawTargetLockWidget->AddToViewport();
	}
	else
	{
		Debug::Print(TEXT("锁定控件创建失败（OwningPlayer 无效或控件类不可用）"));
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	// GetHeroController() 为空时 ProjectWorldLocationToWidgetPosition 会崩
	if (!DrawTargetLockWidget || !CurrentLockActor || !GetHeroController())
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroController(),
		CurrentLockActor->GetActorLocation(),
		ScreenPosition,
		true);

	if (WidgetSize == FVector2D::ZeroVector && DrawTargetLockWidget->WidgetTree)
	{
		DrawTargetLockWidget->WidgetTree->ForEachWidget([this](UWidget* Widget)
	   {
	 	    if (USizeBox* FoundBox = Cast<USizeBox>(Widget))
	    	{
		    	WidgetSize.X = FoundBox->GetWidthOverride();
		    	WidgetSize.Y = FoundBox->GetHeightOverride();
		    }
	    });
		
	}
	ScreenPosition -= (WidgetSize/2.f);
	DrawTargetLockWidget->SetPositionInViewport(ScreenPosition,false);
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	ResetTargetMovement();
	CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	AvailableActorsToLock.Empty();
	CurrentLockActor = nullptr;

	if (DrawTargetLockWidget)
	{
		DrawTargetLockWidget->RemoveFromParent();
		DrawTargetLockWidget = nullptr;
	}
	WidgetSize = FVector2D::ZeroVector;
	
	if (CachedDefaultMaxWalkSpeed > 0.f && GetHeroCharacter() && GetHeroCharacter()->GetCharacterMovement())
	{
		GetHeroCharacter()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}

	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed = GetHeroCharacter()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacter()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	if (!GetHeroController() || !GetHeroController()->IsLocalPlayerController())
	{
		return;
	}

	ULocalPlayer* LocalPlayer = GetHeroController()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!Subsystem)
	{
		return;
	}

	Subsystem->AddMappingContext(TargetLockInputMappingContext, 0);
}

void UHeroGameplayAbility_TargetLock::ResetTargetMovement()
{
	if (CachedDefaultMaxWalkSpeed > 0.f)
	{
		GetHeroCharacter()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}

}

void UHeroGameplayAbility_TargetLock::ResetTargetMappingContext()
{
	if (!GetHeroController() || !GetHeroController()->IsLocalPlayerController())
	{
		return;
	}

	ULocalPlayer* LocalPlayer = GetHeroController()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!Subsystem)
	{
		return;
	}

	Subsystem->RemoveMappingContext(TargetLockInputMappingContext);
}
