// MyRPGDemo

#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"

#include "NavigationSystem.h"
#include "WarriorDebugHelper.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"

UAbilityTask_WaitSpawnEnemies::UAbilityTask_WaitSpawnEnemies()
{
}

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(
	UGameplayAbility* OwningAbility,
	FGameplayTag EventTag,
	TSoftClassPtr<AWarriorEnemyCharacter> EnemyClassToSpawn,
	int32 NumToSpawn,
	const FVector& SpawnOrigin,
	float RandomSpawnRadius)
{
	UAbilityTask_WaitSpawnEnemies* Task = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Task->CachedEventTag = EventTag;
	Task->CachedEnemyClassToSpawn = EnemyClassToSpawn;
	Task->CachedNumToSpawn = NumToSpawn;
	Task->CachedSpawnOrigin = SpawnOrigin;
	Task->CachedRandomSpawnRadius = RandomSpawnRadius;
	return Task;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	DelegateHandle = Delegate.AddUObject(this,&ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	Delegate.Remove(DelegateHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* Payload)
{
	if (ensure(!CachedEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(CachedEnemyClassToSpawn.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded));
		
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}

		EndTask();
	}
	
}

void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	UClass* LoadedClass = CachedEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();
	if (!World || !LoadedClass)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		EndTask();
		return;
	}

	TArray<AWarriorEnemyCharacter*> SpawnedEnemies;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this,CachedSpawnOrigin,RandomLocation,CachedRandomSpawnRadius);
		RandomLocation += FVector(0.f,0.f,150.f);

		const FRotator SpawnFacingRotator = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().Rotation();
		AWarriorEnemyCharacter* SpawnedEnemy = World->SpawnActor<AWarriorEnemyCharacter>(LoadedClass,RandomLocation,SpawnFacingRotator,SpawnParams);

		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnedEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
	}
	EndTask();
}
