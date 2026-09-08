// MyRPGDemo


#include "AbilitySystem/Abilities/HeroGameplayAbility_Contact.h"

#include "Items/PickUps/WarriorStoneBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGameplayAbility_Contact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GetHeroUIComponent()->OnInteract.Broadcast(true);
}

void UHeroGameplayAbility_Contact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	GetHeroUIComponent()->OnInteract.Broadcast(false);
}

void UHeroGameplayAbility_Contact::CollectStones()
{
	CollectedStones.Empty();
	
	TArray<FHitResult> TraceHit;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetAvatarActorFromActorInfo(),
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		GetAvatarActorFromActorInfo()->GetActorLocation() + GetAvatarActorFromActorInfo()->GetActorUpVector() * BoxTraceDistance,
		TraceBoxSize / 2,
		(GetAvatarActorFromActorInfo()->GetActorLocation()).ToOrientationRotator(),
		StoneTraceChannel,
		false,
		TArray<AActor*>(),
		bDrawPersistenDebugShape? EDrawDebugTrace::ForOneFrame :EDrawDebugTrace::None,
		TraceHit,
		true
	);

	for (const FHitResult& Hit : TraceHit)
	{
		if (AWarriorStoneBase* HitStone= Cast<AWarriorStoneBase>(Hit.GetActor()))
		{
			CollectedStones.AddUnique(HitStone);
		}
	}

	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}

void UHeroGameplayAbility_Contact::ConsumeStones()
{
	if (CollectedStones.IsEmpty())
	{
		return;
	}

	for (AWarriorStoneBase* Stone : CollectedStones)
	{
		if (Stone)
		{
			Stone->Consume(GetWarriorAbilitySystemComponent(), GetAbilityLevel());
		}
	}
}
