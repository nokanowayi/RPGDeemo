// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorTypes/WarriorEnumTypes.h"
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

public:
	/** 判断是否为 OnGiven 激活策略（客户端初始化时需要激活这类能力） */
	FORCEINLINE bool IsOnGivenActivationPolicy() const
	{
		return AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven;
	}

protected:

	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponent() const;

	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const;

	FActiveGameplayEffectHandle NativeApplyGameplaySpecEffectHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle) const;
	
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability",meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "SuccessType"))
	FActiveGameplayEffectHandle BP_ApplyGameplaySpecEffectHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle,EWarriorSuccessType& SuccessType);
};
