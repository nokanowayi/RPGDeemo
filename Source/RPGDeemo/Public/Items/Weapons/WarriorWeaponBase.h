// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

UCLASS()
class RPGDEEMO_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarriorWeaponBase();
protected:
	UPROPERTY(visibleAnywhere,BlueprintReadOnly,Category = "Weapons")
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	UBoxComponent* WeaponCollisionBox;
public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox; }
	
};
