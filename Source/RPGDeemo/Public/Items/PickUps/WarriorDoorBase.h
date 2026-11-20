// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Items/PickUps/WarriorPickUpBase.h"
#include "WarriorDoorBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API AWarriorDoorBase : public AWarriorPickUpBase
{
	GENERATED_BODY()
public:
	void Interact(const FString& LevelName);
	
protected:
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
