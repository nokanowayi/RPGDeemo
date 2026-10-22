// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_Dissolve.generated.h"

class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityTaskDissolveFinishedDelegate);

/**
 * 溶解渐变任务：每帧把 Mesh 材质的 DissolveAmount 参数从 0 渐变到 1，
 * 播完后回调 OnFinished 并结束任务（配合死亡 GA 使用）
 */
UCLASS()
class RPGDEEMO_API UAbilityTask_Dissolve : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_Dissolve();
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_Dissolve* CreateDissolveTask(UGameplayAbility* OwningAbility, USkeletalMeshComponent* Mesh, float Duration, FName ParameterName = TEXT("DissolveAmount"));

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	/** 溶解完成（Alpha 到达 1）时触发 */
	UPROPERTY(BlueprintAssignable)
	FOnAbilityTaskDissolveFinishedDelegate OnFinished;

private:
	/** 目标网格（溶解对象） */
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> Mesh;

	/** 为 Mesh 各材质槽创建的动态材质实例（静态 MI 无法直接设置参数，必须走 MID） */
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
	
	float Duration = 2.f;
	
	float Elapsed = 0.f;
	
	FName ParameterName = TEXT("DissolveAmount");
};
