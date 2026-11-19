// MyRPGDemo


#include "Items/PickUps/WarriorStoneBase.h"

#include "WarriorGameplayTags.h"
#include "Characters/WarriorHeroCharacter.h"

void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	if (!HasAuthority() || bConsumed || !IsValid(AbilitySystemComponent) || !StoneGameplayEffect)
	{
		return;
	}

	const AActor* Recipient = AbilitySystemComponent->GetOwnerActor();
	if (!IsValid(Recipient) || !Recipient->HasAuthority())
	{
		return;
	}

	// Claim before applying the effect so overlapping requests cannot consume it twice.
	bConsumed = true;
	const FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(
		StoneGameplayEffect.GetDefaultObject(), ApplyLevel, AbilitySystemComponent->MakeEffectContext());
	if (!EffectHandle.WasSuccessfullyApplied())
	{
		bConsumed = false;
		return;
	}

	SetActorEnableCollision(false);
	BP_OnStoneConsumed();
	if (!IsActorBeingDestroyed())
	{
		Destroy();
	}
}

void AWarriorStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                            AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                            const FHitResult& SweepResult)
{
	if (AWarriorHeroCharacter* WarriorHeroCharacter = Cast<AWarriorHeroCharacter>(OtherActor))
	{
		WarriorHeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGameplayTags::Player_Ability_Interact);
	}
}
