// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

class UGameplayAbility;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void OnAbilityInputPressed(FGameplayTag& InputTag);
	void OnAbilityInputReleased(FGameplayTag& InputTag);
	
	virtual float PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, float StartTimeSeconds = 0.0f) override;
	
	virtual float PlayMontageSimulated(UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None) override;
	
	virtual void OnRep_ReplicatedAnimMontage() override;

	UFUNCTION(blueprintCallable,Category="Warrior/Ability",meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& WeaponAbilities,const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities,int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpeHandles);

	UFUNCTION(blueprintCallable,Category="Warrior/Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpeHandlesToRemove);

	UFUNCTION(blueprintCallable,Category="Warrior/Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
