// MyRPGDemo


#include "Component/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
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

		const bool BIsPlayerBlocking = false;
		const bool BIsMyAttackingUnblockable = false;

		if (BIsPlayerBlocking && !BIsMyAttackingUnblockable)
		{
			
		}

		FGameplayEventData EventData;
		EventData.Instigator = GetOwningPawn();
		EventData.Target = HitActor;
		
		if (BIsValidBlock)
		{
			
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
