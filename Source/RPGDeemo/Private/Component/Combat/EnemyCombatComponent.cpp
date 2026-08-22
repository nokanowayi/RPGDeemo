// MyRPGDemo


#include "Component/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (HitActor)
	{
		if (OverlapActors.Contains(HitActor))
		{
			return;
		}

		OverlapActors.AddUnique(HitActor);

		bool BIsValidBlock = false;

		const bool BIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor, WarriorGameplayTags::Player_Status_Blocking);
		const bool BIsMyAttackingUnblockable = false;

		if (BIsPlayerBlocking && !BIsMyAttackingUnblockable)
		{
			BIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
		}

		FGameplayEventData EventData;
		EventData.Instigator = GetOwningPawn();
		EventData.Target = HitActor;
		
		if (BIsValidBlock)
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				HitActor,
				WarriorGameplayTags::Player_Event_SuccessfulBlock,
				EventData
				);
		}
		else
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				GetOwningPawn(),
				WarriorGameplayTags::Shared_Event_MeleeHit,
				EventData
				);
		}
	}
}
