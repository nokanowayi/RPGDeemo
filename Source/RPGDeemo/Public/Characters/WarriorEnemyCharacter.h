// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "Component/Combat/EnemyCombatComponent.h"
#include "WarriorEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	AWarriorEnemyCharacter();

	virtual  UPawnCombatComponent* GetPawnCombatComponent() const override;
protected:
	virtual void PossessedBy(AController* NewController) override;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

	private:
	void InitEnemyStartUpData();
public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const {return EnemyCombatComponent; };
};
