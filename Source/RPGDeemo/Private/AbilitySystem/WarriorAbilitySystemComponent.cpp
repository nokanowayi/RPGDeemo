// MyRPGDemo

#include "AbilitySystem//Abilities/WarriorHeroGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

#include "WarriorDebugHelper.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (const FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(FGameplayTag& InputTag)
{
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& WeaponAbilities,
	int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpeHandles)
{
	if (WeaponAbilities.IsEmpty())
	{
		return;
	}
	for (const FWarriorHeroAbilitySet& HeroAbilitySet : WeaponAbilities)
	{
		if (!HeroAbilitySet.IsValid()) continue;
		FGameplayAbilitySpec AbilitySpec(HeroAbilitySet.AbilityToGrant, ApplyLevel);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(HeroAbilitySet.InputTag);
		
		OutGrantedAbilitySpeHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpeHandlesToRemove)
{
	if (InSpeHandlesToRemove.IsEmpty()) return;
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpeHandlesToRemove)
	{
		Debug::Print(FString::Printf(TEXT("RemoveGrantedHeroWeaponAbilities: %s"), *SpecHandle.ToString()));
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
		InSpeHandlesToRemove.Empty();
	}
}

