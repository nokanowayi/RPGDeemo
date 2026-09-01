// MyRPGDemo


#include "Component/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Characters/WarriorEnemyCharacter.h"

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
		const bool BIsMyAttackingUnblockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(), WarriorGameplayTags::Enemy_Status_UnBlockable);

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

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	Super::ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);


	AWarriorEnemyCharacter* OwningEnemyCharacter = GetOwningPawn<AWarriorEnemyCharacter>();

	UBoxComponent* LeftHandClo = OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHandClo = OwningEnemyCharacter->GetRightHandCollisionBox();

	if (ToggleDamageType == EToggleDamageType::LeftHand)
	{
		LeftHandClo->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
	else if (ToggleDamageType == EToggleDamageType::RightHand)
	{
		RightHandClo->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	if (bShouldEnable == false)
	{
		OverlapActors.Empty();
	}
}
