// MyRPGDemo

#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem//Abilities/WarriorHeroGameplayAbility.h"

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

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),AbilitySpecs);

	if (AbilitySpecs.Num() != 0)
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, AbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = AbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}


	}
	return false;
}

