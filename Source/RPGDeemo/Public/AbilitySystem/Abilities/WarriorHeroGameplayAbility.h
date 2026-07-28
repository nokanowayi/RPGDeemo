// MyRPGDemo

#pragma once

#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"
#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "Component/Combat/HeroCombatComponent.h"
#include "WarriorHeroGameplayAbility.generated.h"


/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	AWarriorHeroCharacter* GetHeroCharacter();

	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
    AWarriorHeroController* GetHeroController();

	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UHeroCombatComponent* GetHeroCombatComponent();
	
private:
	TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
	TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;
};
