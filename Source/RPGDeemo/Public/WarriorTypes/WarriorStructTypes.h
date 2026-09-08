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
	TSubclassOf<UWarriorHeroGameplayAbility> AbilityToGrant;
	bool IsValid() const;
};

USTRUCT(blueprintType)
struct FWarriorHeroSpecialAbilitySet:public FWarriorHeroAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface> AbilityIconMaterial;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AbilityCooldownTag;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroSpecialAbilitySet> SpecialWeaponAbilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};
