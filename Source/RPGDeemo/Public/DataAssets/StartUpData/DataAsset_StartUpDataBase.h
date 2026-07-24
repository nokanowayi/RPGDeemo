// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAsset_StartUpDataBase.generated.h"


/**
 * 
 */
UCLASS()
class RPGDEEMO_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);
protected:
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateOnGivenAbilities;
	
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

	void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>> InAbilityToGive,UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);
};
