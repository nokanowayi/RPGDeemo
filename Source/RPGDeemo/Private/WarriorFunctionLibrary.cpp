// MyRPGDemo

#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorFunctionLibrary.h"
#include "SaveGame/WarriorSaveGame.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/MeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Settings/FrontendDeveloperSettings.h"
#include "GameStates/WarriorSurvialGameState.h"
#include "WarriorTypes/WarriorCountDownAction.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	
	return  CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{

	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
		ASC->AddReplicatedLooseGameplayTag(TagToAdd);
	}
	
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->RemoveLooseGameplayTag(TagToAdd);
		ASC->RemoveReplicatedLooseGameplayTag(TagToAdd);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor,FGameplayTag TagToCheck)
{
	return NativeGetWarriorASCFromActor(InActor)->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor,TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (Cast<IPawnCombatInterface>(InActor))
	{
		return Cast<IPawnCombatInterface>(InActor)->GetPawnCombatComponent();
	}
	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent?EWarriorValidType::Valid:EWarriorValidType::Invalid;
	return CombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	if (!QueryPawn || !TargetPawn)
	{
		return false;
	}

	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());
	
	if (QueryTeamAgent && TargetTeamAgent)
	{
		return  QueryTeamAgent->GetGenericTeamId() == TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, int32 InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirTag(AActor* InAttacker, AActor* InVictim,
	float& OutAngelDifference)
{
	check(InAttacker && InVictim);
	FVector VictimForward = InVictim->GetActorForwardVector();
	FVector VectorNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotRes = FVector::DotProduct(VictimForward, VectorNormalized);
	OutAngelDifference = UKismetMathLibrary::DegAcos(DotRes);

	const FVector CrossRes = FVector::CrossProduct(VictimForward, VectorNormalized);

	if (CrossRes.Z < 0.f)
	{
		OutAngelDifference = -OutAngelDifference;
	}

	if (OutAngelDifference >= -45.f && OutAngelDifference < 45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngelDifference >= 45.f && OutAngelDifference < 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}
	else if (OutAngelDifference >= -135.f && OutAngelDifference < -45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngelDifference >= 135.f || OutAngelDifference < -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	
	return WarriorGameplayTags::Shared_Status_HitReact_Front;
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);
	const float DotRes = FVector::DotProduct(InDefender->GetActorForwardVector(), InAttacker->GetActorLocation());

	
	return DotRes < 0.1f;
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
	const FGameplayEffectSpecHandle InSpecHandle)
{
	// 句柄为空时 *InSpecHandle.Data 会直接触发 SharedPointer.h 的 check(IsValid()) 断言崩编辑器，
	// 这里改成可诊断的失败返回
	if (!InSpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[ApplyGE] SpecHandle 无效（Data 为空）：Instigator=%s Target=%s"),
			*GetNameSafe(InInstigator), *GetNameSafe(InTargetActor));
		return false;
	}

	// 不能用 NativeGetWarriorASCFromActor：它内部是 CastChecked，
	// 命中一个没有 ASC 的 Pawn 会在这里再断言崩一次
	UWarriorAbilitySystemComponent* InstigatorASC = Cast<UWarriorAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InInstigator));
	UWarriorAbilitySystemComponent* TargetASC = Cast<UWarriorAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetActor));

	if (!InstigatorASC || !TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ApplyGE] 缺少 ASC：Instigator=%s(%s) Target=%s(%s)"),
			*GetNameSafe(InInstigator), InstigatorASC ? TEXT("有") : TEXT("无"),
			*GetNameSafe(InTargetActor), TargetASC ? TEXT("有") : TEXT("无"));
		return false;
	}

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = InstigatorASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput,
	UPARAM(DisplayName = "OutPut")EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	FWarriorCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget,LatentInfo.UUID);

	if (CountDownInput == EWarriorCountDownActionInput::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FWarriorCountDownAction(TotalTime,UpdateInterval,OutRemainingTime,CountDownOutput,LatentInfo));
		}
	}
	if (CountDownInput == EWarriorCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}

	
}

UWarriorGameInstance* UWarriorFunctionLibrary::GetGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
		{
			return Cast<UWarriorGameInstance>(World->GetGameInstance());
		}
		
	}

	return nullptr;
}

AWarriorSurvialGameState* UWarriorFunctionLibrary::GetSurvialGameState(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetGameState<AWarriorSurvialGameState>();
		}
	}

	return nullptr;
}

void UWarriorFunctionLibrary::ToogleInputMode(EWarriorInputMode InputMode, const UObject* WorldContextObject)
{
	APlayerController* PlayerController = nullptr;
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController = World->GetFirstPlayerController();
		}
		
	}

	if (!PlayerController)
	{
		return;
	}

	FInputModeGameOnly InputModeGameOnly;
	FInputModeUIOnly InputModeUIOnly;

	switch (InputMode)
	{
		case EWarriorInputMode::GameOnly:
			PlayerController->SetInputMode(InputModeGameOnly);
			PlayerController->bShowMouseCursor = false;
			break;
		case EWarriorInputMode::UIOnly:
			PlayerController->SetInputMode(InputModeUIOnly);
			PlayerController->bShowMouseCursor = true;
			break;
	}
}

void UWarriorFunctionLibrary::SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficulty)
{
	USaveGame* SaveGameObject =UGameplayStatics::CreateSaveGameObject(UWarriorSaveGame::StaticClass());
	if (UWarriorSaveGame* WarriorSaveGame = Cast<UWarriorSaveGame>(SaveGameObject))
	{
		WarriorSaveGame->SaveCurrentGameDifficulty = InDifficulty;
		bool IsSaved = UGameplayStatics::SaveGameToSlot(WarriorSaveGame, WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);
		Debug::Print(IsSaved ? TEXT("Save Success") : TEXT("Save Failed"));
	}
}

bool UWarriorFunctionLibrary::TryLoadSaveCurrentGameDifficulty(EWarriorGameDifficulty& OutDifficulty)
{
	if (UGameplayStatics::DoesSaveGameExist(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0))
	{
		USaveGame* SaveGameObject =UGameplayStatics::LoadGameFromSlot(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);
		if (UWarriorSaveGame* WarriorSaveGame = Cast<UWarriorSaveGame>(SaveGameObject))
		{
			OutDifficulty = WarriorSaveGame->SaveCurrentGameDifficulty;
			Debug::Print(TEXT("Loaded"));
			return true;
		}
	}
	return false;
}

TSoftClassPtr<UWidget_ActivatableBase> UWarriorFunctionLibrary::GetFrontendWidgetClassByTag(UPARAM (meta = (Categories = "UI.Widget"))FGameplayTag InTag)
{
	const UFrontendDeveloperSettings* Settings = GetDefault<UFrontendDeveloperSettings>();

	return  Settings->FrontendWidgetMap.FindRef(InTag);
}

void UWarriorFunctionLibrary::StartDissolveOnActor(AActor* TargetActor, float Duration, FName ParameterName)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	UWorld* World = TargetActor->GetWorld();
	if (!World)
	{
		return;
	}
	
	TArray<TWeakObjectPtr<UMeshComponent>> Meshes;
	{
		TArray<UMeshComponent*> FoundMeshes;
		TargetActor->GetComponents<UMeshComponent>(FoundMeshes);

		for (UMeshComponent* Mesh : FoundMeshes)
		{
			if (IsValid(Mesh))
			{
				// 复用的材质实例上可能残留上一次的溶解进度，先重置
				Mesh->SetScalarParameterValueOnMaterials(ParameterName, 0.f);
				Meshes.Add(Mesh);
			}
		}
	}
	
	for (const TWeakObjectPtr<UMeshComponent>& WeakMesh : Meshes)
	{
		if (UMeshComponent* Mesh = WeakMesh.Get())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Dissolve]   Mesh=%s Slots=%d"), *Mesh->GetName(), Mesh->GetNumMaterials());
		}
	}

	const float SafeDuration = FMath::Max(0.01f, Duration);
	constexpr float UpdateStep = 0.02f;
	
	// 进度与 TimerHandle 用共享指针保存：lambda 内部可以自行结束定时器，
	TSharedPtr<float> Elapsed = MakeShared<float>(0.f);
	TSharedPtr<FTimerHandle> TimerHandle = MakeShared<FTimerHandle>();
	TWeakObjectPtr<AActor> WeakActor(TargetActor);

	FTimerDelegate Delegate = FTimerDelegate::CreateWeakLambda(World,
		[WeakActor, Meshes, ParameterName, SafeDuration, Elapsed, TimerHandle, World]()
		{
			if (!WeakActor.IsValid())
			{
				World->GetTimerManager().ClearTimer(*TimerHandle);
				return;
			}

			*Elapsed += UpdateStep;
			const float Alpha = FMath::Clamp(*Elapsed / SafeDuration, 0.f, 1.f);

			for (const TWeakObjectPtr<UMeshComponent>& WeakMesh : Meshes)
			{
				if (UMeshComponent* Mesh = WeakMesh.Get())
				{
					// SetScalarParameterValueOnMaterials 会自动为静态材质创建 MID
					Mesh->SetScalarParameterValueOnMaterials(ParameterName, Alpha);
				}
			}

			if (Alpha >= 1.f)
			{
				World->GetTimerManager().ClearTimer(*TimerHandle);
			}
		});

	World->GetTimerManager().SetTimer(*TimerHandle, Delegate, UpdateStep, true);
}

void UWarriorFunctionLibrary::ApplyRollMovement(AActor* TargetActor, FVector WorldDirection, float Speed, float Duration)
{

	if (!TargetActor)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(TargetActor);
	UCharacterMovementComponent* MoveComp = Character ? Character->GetCharacterMovement() : nullptr;
	if (!MoveComp)
	{
		return;
	}

	UWorld* World = TargetActor->GetWorld();
	if (!World || Duration <= KINDA_SMALL_NUMBER || Speed <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// 方向归一化
	FVector Direction = WorldDirection.GetSafeNormal();
	if (Direction.IsNearlyZero() || Direction.ContainsNaN())
	{
		Direction = Character->GetActorForwardVector().GetSafeNormal();
	}
	if (Direction.IsNearlyZero() || Direction.ContainsNaN())
	{
		return;
	}

	const bool bIsAuthority = TargetActor->HasAuthority();
	const bool bIsLocallyControlled = Character->IsLocallyControlled();
	const float TotalDistance = Speed * Duration;

	// 权威端直接采用客户端上报的位置
	if (bIsAuthority)
	{
		MoveComp->bIgnoreClientMovementErrorChecksAndCorrection = true;
		MoveComp->bServerAcceptClientAuthoritativePosition = true;

		TWeakObjectPtr<UCharacterMovementComponent> WeakMoveCompRef(MoveComp);
		FTimerHandle RestoreHandle;
		FTimerDelegate RestoreDelegate = FTimerDelegate::CreateWeakLambda(World,
			[WeakMoveCompRef]()
			{
				if (UCharacterMovementComponent* MC = WeakMoveCompRef.Get())
				{
					MC->bIgnoreClientMovementErrorChecksAndCorrection = false;
					MC->bServerAcceptClientAuthoritativePosition = false;
				}
			});
		World->GetTimerManager().SetTimer(RestoreHandle, RestoreDelegate, Duration + 0.15f, false);
	}
	
	if (!bIsLocallyControlled)
	{
		return;
	}

	const FVector StartLocation = TargetActor->GetActorLocation();
	const FRotator FaceRotation(0.f, Direction.Rotation().Yaw, 0.f);

	TWeakObjectPtr<AActor> WeakActor(TargetActor);
	TSharedPtr<float> Elapsed = MakeShared<float>(0.f);
	TSharedPtr<FTimerHandle> TimerHandle = MakeShared<FTimerHandle>();

	// 每一帧独自推进位移和确定方向
	FTimerDelegate StepDelegate = FTimerDelegate::CreateWeakLambda(World,
		[WeakActor, Direction, FaceRotation, Speed, Duration, TotalDistance, Elapsed, TimerHandle, World, StartLocation]()
		{
			AActor* Actor = WeakActor.Get();
			if (!Actor)
			{
				World->GetTimerManager().ClearTimer(*TimerHandle);
				return;
			}

			const float DeltaSeconds = FMath::Max(World->GetDeltaSeconds(), 1.f / 240.f);
			
			Actor->SetActorRotation(FaceRotation);
			
			FHitResult Hit;
			Actor->AddActorWorldOffset(Direction * Speed * DeltaSeconds, /*bSweep=*/true, &Hit);

			*Elapsed += DeltaSeconds;
			if (Hit.bBlockingHit || *Elapsed >= Duration)
			{
				World->GetTimerManager().ClearTimer(*TimerHandle);
				
			}
		});
	World->GetTimerManager().SetTimer(*TimerHandle, StepDelegate, 1.f / 60.f, true);
}
