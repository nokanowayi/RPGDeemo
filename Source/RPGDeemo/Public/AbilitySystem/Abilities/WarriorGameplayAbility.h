// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;

UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy:uint8
{
	OnTrigger,
	OnGiven
};
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly,Category="WarriorAbility")
	EWarriorAbilityActivationPolicy AbilityActivationPolicy = EWarriorAbilityActivationPolicy::OnTrigger;
	
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponent() const;

	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const;
};
