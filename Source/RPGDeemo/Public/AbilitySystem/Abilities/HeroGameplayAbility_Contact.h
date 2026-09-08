// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Items/PickUps/WarriorStoneBase.h"
#include "HeroGameplayAbility_Contact.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UHeroGameplayAbility_Contact : public UWarriorHeroGameplayAbility
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
	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	float BoxTraceDistance = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	FVector TraceBoxSize = FVector(100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	TArray<TEnumAsByte<EObjectTypeQuery>> StoneTraceChannel;

	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	bool bDrawPersistenDebugShape = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Contact Ability")
	TArray<AWarriorStoneBase*> CollectedStones;
};
