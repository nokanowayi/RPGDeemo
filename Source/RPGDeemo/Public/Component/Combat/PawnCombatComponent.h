// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Component/PawnExtensionComponentBase.h"
#include "PawnCombatComponent.generated.h"

UENUM()
enum class EToggleDamageType:uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand,
};
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
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	UFUNCTION(blueprintCallable,category="Warrior|Combat")
	void ToggleWeaponColiision(bool bShouldEnable,EToggleDamageType ToggleDamageType = EToggleDamageType::CurrentEquippedWeapon);

	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPullFromTargetActor(AActor* InteractedActor);
protected:
	TArray<AActor*> OverlapActors;
private:
	TMap<FGameplayTag,AWarriorWeaponBase*> CharacterCarriedWeaponsMap;
};
