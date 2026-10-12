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

	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& SwitchDirectionTag);
private:
	// 返回是否成功锁到目标：没锁到时内部已经 Cancel 掉能力，调用方必须立刻返回，
	// 否则会继续执行 InitTargetLockMovement 等逻辑（能力已结束，没人会去恢复速度）
	bool TryLockOnTarget();
	void GetAvailableActorsToLock();

	void GetAvailableActorsAroundTaget(TArray<AActor*>& OutActorsOnLeft,TArray<AActor*>& OutActorsOnRight);
	AActor* GetClosestActorToLock();
	AActor* GetClosestActorToLock(TArray<AActor*> ActorsToCheck);
	void DrawTargetWidget();
	void SetTargetLockWidgetPosition();
	void InitTargetLockMovement();
	void InitTargetLockMappingContext();
	

	void CancelTargetLockAbility();
	void CleanUp();
	void ResetTargetMovement();
	void ResetTargetMappingContext();

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

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	UInputMappingContext* TargetLockInputMappingContext;

	UPROPERTY(EditDefaultsOnly,category="TargetLock")
	float TargetLockCameraDis = 20.f;
	
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
