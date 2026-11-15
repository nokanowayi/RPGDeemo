// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_Interact.generated.h"

class AWarriorStoneBase;

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UHeroGameplayAbility_Interact : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(blueprintCallable)
	void CollectStones();

	UFUNCTION(blueprintCallable)
	void ConsumeStones();
private:
	void SetInteractionPromptVisible(bool bVisible);

	bool bInteractionPromptVisible = false;

	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	float BoxTraceDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	FVector TraceBoxSize = FVector(100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	TArray<TEnumAsByte<EObjectTypeQuery>> StoneTraceChannel;

	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	bool bDrawDebugShape = false;

	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	TArray<AWarriorStoneBase*> CollectedStones;

};
