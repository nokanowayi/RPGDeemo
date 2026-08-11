// MyRPGDemo

#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "AbilitySystem//Abilities/WarriorHeroGameplayAbility.h"


void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
	int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
	for (const FWarriorHeroAbilitySet& HeroAbilitySet : HeroStartUpAbilitySets)
	{
		if (!HeroAbilitySet.IsValid()) continue;
		FGameplayAbilitySpec AbilitySpec(HeroAbilitySet.AbilityToGrant, ApplyLevel);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(HeroAbilitySet.InputTag);
		
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
