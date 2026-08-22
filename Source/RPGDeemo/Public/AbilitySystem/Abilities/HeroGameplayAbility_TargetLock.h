// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void TryLockOnTarget();
	void GetAvaliableActorsToLock();

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	float TraceDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	FVector TraceBoxSize = FVector(5000.f,5000.f,300.f);
	
	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceChannel;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	bool bShowPersistentDebugShape = false;

	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;
};
