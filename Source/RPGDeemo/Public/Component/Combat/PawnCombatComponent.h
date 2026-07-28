// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Component/PawnExtensionComponentBase.h"
#include "PawnCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
public:
	UFUNCTION(blueprintCallable,category="Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,AWarriorWeaponBase* InWeaponToRegister,bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(blueprintCallable,category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(blueprintCallable,category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon(FGameplayTag InWeaponTagToGet) const;
private:
	TMap<FGameplayTag,AWarriorWeaponBase*> CharacterCarriedWeaponsMap;
};
