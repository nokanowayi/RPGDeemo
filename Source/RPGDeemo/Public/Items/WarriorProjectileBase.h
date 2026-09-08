// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WarriorProjectileBase.generated.h"

UENUM(BlueprintType)
enum class EProjectileDamagePolicy:uint8
{
	OnHit,
	OnBeginOverlap,
};
UCLASS()
class RPGDEEMO_API AWarriorProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWarriorProjectileBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Projectile")
	UBoxComponent* ProjectileCollisionBox;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Projectile")
	UNiagaraComponent* ProjectileNiagaraComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Projectile")
	EProjectileDamagePolicy DamagePolicy = EProjectileDamagePolicy::OnHit;

	UPROPERTY(BlueprintReadOnly,Category = "Projectile",meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle;

	UFUNCTION(BlueprintImplementableEvent,Category = "Projectile",meta=( DisplayName = "On Spawn ProjectileHit FX"))
	void BP_OnSpawnProjectileHitFX(const FVector& HitLocation);

	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent , AActor* OtherActor ,UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& Data);
	TArray<AActor*> OverlappedActors;
};
