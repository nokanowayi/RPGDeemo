// MyRPGDemo

#pragma once
#include "GameplayTagContainer.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AnimInstances/Hero/WarriHeroLinkedAnimLayer.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"

#include "WarriorStructTypes.generated.h"
class UWarriorHeroGameplayAbility;

USTRUCT(blueprintType)
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;
	bool IsValid() const;
};

USTRUCT(Blueprintable)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	TSubclassOf<UWarriHeroLinkedAnimLayer> WeaponAnimLayerToLink;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;
};
