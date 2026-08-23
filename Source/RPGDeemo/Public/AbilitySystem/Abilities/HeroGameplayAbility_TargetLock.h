// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Widget/WarriorWidgetBase.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);
private:
	void TryLockOnTarget();
	void GetAvailableActorsToLock();

	AActor* GetClosestActorToLock();
	void DrawTargetWidget();
	void SetTargetLockWidgetPosition();
	void InitTargetLockMovement();

	void CancelTargetLockAbility();
	void CleanUp();
	void ResetTargetMovement();

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	float TraceDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	FVector TraceBoxSize = FVector(5000.f,5000.f,300.f);
	
	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceChannel;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	bool bShowPersistentDebugShape = false;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	float TargetLockCameraSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	float TargetLockMaxWalkSpeed = 150.f;
	
	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;
	
	UPROPERTY()
	AActor* CurrentLockActor;

	UPROPERTY()
	UWarriorWidgetBase* DrawTargetLockWidget;

	UPROPERTY()
	FVector2D WidgetSize = FVector2D::ZeroVector;

	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
	
};
