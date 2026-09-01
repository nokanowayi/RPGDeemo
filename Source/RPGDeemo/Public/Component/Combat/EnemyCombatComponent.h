// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "EnemyCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UEnemyCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

	public:
	virtual void OnHitTargetActor(AActor* HitActor) override;

protected:
	virtual void ToggleBodyCollisionBoxCollision(bool bShouldEnable,EToggleDamageType ToggleDamageType) override;
};
