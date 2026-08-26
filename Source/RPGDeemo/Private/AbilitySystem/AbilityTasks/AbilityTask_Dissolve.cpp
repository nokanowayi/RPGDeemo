// MyRPGDemo

#include "AbilitySystem/AbilityTasks/AbilityTask_Dissolve.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UAbilityTask_Dissolve::UAbilityTask_Dissolve()
{
	bTickingTask = true;
}

UAbilityTask_Dissolve* UAbilityTask_Dissolve::CreateDissolveTask(UGameplayAbility* OwningAbility,
	USkeletalMeshComponent* InMesh, float InDuration, FName InParameterName)
{
	UAbilityTask_Dissolve* Task = NewAbilityTask<UAbilityTask_Dissolve>(OwningAbility);
	Task->Mesh = InMesh;
	Task->Duration = FMath::Max(InDuration, 0.01f);
	Task->ParameterName = InParameterName;
	return Task;
}

void UAbilityTask_Dissolve::Activate()
{
	Super::Activate();

	Elapsed = 0.f;
	DynamicMaterials.Reset();

	if (!Mesh || !IsValid(Mesh))
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	// 为每个材质槽创建动态材质实例（静态 MI 无法直接设置参数，必须走 MID）
	const int32 NumMaterials = Mesh->GetNumMaterials();
	for (int32 SlotIndex = 0; SlotIndex < NumMaterials; ++SlotIndex)
	{
		if (UMaterialInterface* Material = Mesh->GetMaterial(SlotIndex))
		{
			if (UMaterialInstanceDynamic* MID = Mesh->CreateDynamicMaterialInstance(SlotIndex))
			{
				DynamicMaterials.Add(MID);
				// 初始 Alpha = 0（未溶解）
				MID->SetScalarParameterValue(ParameterName, 0.f);
			}
		}
	}

	if (DynamicMaterials.IsEmpty())
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}
}

void UAbilityTask_Dissolve::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!Mesh || !IsValid(Mesh))
	{
		OnFinished.Broadcast();
		EndTask();
		return;
	}

	Elapsed += DeltaTime;
	const float Alpha = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);

	for (UMaterialInstanceDynamic* MID : DynamicMaterials)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(ParameterName, Alpha);
		}
	}

	if (Alpha >= 1.f)
	{
		OnFinished.Broadcast();
		EndTask();
	}
}
