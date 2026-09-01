// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate,const TArray<AWarriorEnemyCharacter*>&, SpawnedEnemies);
/**
 * 等待 GameplayEvent 并生成指定数量的敌人（用于 Boss 战召唤小怪）
 */
UCLASS()
class RPGDEEMO_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_WaitSpawnEnemies();

	UFUNCTION(BlueprintCallable,Category="Ability|Tasks",meta=(DisplayName = "Wait Gameplay Event And Spawn Enemies",HidePin = "OwningAbility",DefaultToSelf = "OwningAbility",BlueprintInternalUseOnly = "true",NumToSpawn = "1",RandomSpawnRadius = "200"))
	static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(UGameplayAbility* OwningAbility, FGameplayTag EventTag,TSoftClassPtr<AWarriorEnemyCharacter> EnemyClassToSpawn, int32 NumToSpawn,const FVector& SpawnOrigin,float RandomSpawnRadius);

	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;

private:
	UPROPERTY()
	FGameplayTag CachedEventTag;

	UPROPERTY()
	TSoftClassPtr<AWarriorEnemyCharacter> CachedEnemyClassToSpawn;


	UPROPERTY()
	int32 CachedNumToSpawn = 1;


	UPROPERTY()
	FVector CachedSpawnOrigin = FVector::ZeroVector;


	UPROPERTY()
	float CachedRandomSpawnRadius = 200.f;


	UPROPERTY()
	FRotator CachedSpawnRotation = FRotator::ZeroRotator;

	FDelegateHandle DelegateHandle;
	void OnGameplayEventReceived(const FGameplayEventData* Payload);
	void OnEnemyClassLoaded();
};
