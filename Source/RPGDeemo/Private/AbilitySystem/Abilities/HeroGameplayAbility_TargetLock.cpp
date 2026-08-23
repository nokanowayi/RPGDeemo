// MyRPGDemo


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"

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
	TryLockOnTarget();
	InitTargetLockMovement();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CleanUp();
	ResetTargetMovement();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockActor || UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockActor,WarriorGameplayTags::Shared_Status_Dead)||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacter(),WarriorGameplayTags::Shared_Status_Dead))
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotation = !UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacter(),WarriorGameplayTags::Player_Status_Rolling)
	&&
	!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacter(),WarriorGameplayTags::Player_Status_Blocking);

	if (bShouldOverrideRotation)
	{
		FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(GetHeroCharacter()->GetActorLocation(),CurrentLockActor->GetActorLocation());

		FRotator ControllerRotator = GetHeroController()->GetControlRotation();

		FRotator TargetRotator = FMath::RInterpTo(ControllerRotator,Rotator,DeltaTime,TargetLockCameraSpeed);

		GetHeroController()->SetControlRotation(FRotator(TargetRotator.Pitch,TargetRotator.Yaw,ControllerRotator.Roll));
		GetHeroCharacter()->SetActorRotation(FRotator(0.f,TargetRotator.Yaw,0.f));
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	CurrentLockActor = GetClosestActorToLock();

	if (CurrentLockActor)
	{
		DrawTargetWidget();
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
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
			if (HitActor != GetHeroCharacter())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetClosestActorToLock()
{
	float Dis = 0.f;
	return  UGameplayStatics::FindNearestActor(GetHeroCharacter()->GetActorLocation(),AvailableActorsToLock,Dis);
}

void UHeroGameplayAbility_TargetLock::DrawTargetWidget()
{
	if (!DrawTargetLockWidget)
	{
		DrawTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroController(),TargetLockWidgetClass);

		DrawTargetLockWidget->AddToViewport();
	}

}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawTargetLockWidget || !CurrentLockActor)
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

	if (WidgetSize == FVector2D::ZeroVector)
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

	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed = GetHeroCharacter()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacter()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::ResetTargetMovement()
{
	if (CachedDefaultMaxWalkSpeed > 0.f)
	{
		GetHeroCharacter()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}

}
