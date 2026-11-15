// MyRPGDemo


#include "AbilitySystem/Abilities/HeroGameplayAbility_Interact.h"

#include "Component/UI/HeroUIComponent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Items/PickUps/WarriorPickUpBase.h"
#include "Items/PickUps/WarriorStoneBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	SetInteractionPromptVisible(false);
	CollectedStones.Reset();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_Interact::SetInteractionPromptVisible(bool bVisible)
{
	if (bInteractionPromptVisible == bVisible)
	{
		return;
	}

	bInteractionPromptVisible = bVisible;
	if (AWarriorHeroCharacter* HeroCharacter = GetHeroCharacter(); IsValid(HeroCharacter))
	{
		if (UHeroUIComponent* UIComponent = HeroCharacter->GetHeroUIComponent(); IsValid(UIComponent))
		{
			UIComponent->OnInteract.Broadcast(bVisible);
		}
	}
}

void UHeroGameplayAbility_Interact::CollectStones()
{
	CollectedStones.Empty();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	TArray<FHitResult> TraceHit;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		AvatarActor,
		AvatarActor->GetActorLocation(),
		AvatarActor->GetActorLocation() + AvatarActor->GetActorUpVector() * BoxTraceDistance,
		TraceBoxSize / 2,
		AvatarActor->GetActorRotation(),
		StoneTraceChannel,
		false,
		TArray<AActor*>(),
		bDrawDebugShape? EDrawDebugTrace::ForOneFrame :EDrawDebugTrace::None,
		TraceHit,
		true
	);

	for (const FHitResult& Hit : TraceHit)
	{
		if (AWarriorStoneBase* HitStone = Cast<AWarriorStoneBase>(Hit.GetActor()); IsValid(HitStone))
		{
			CollectedStones.AddUnique(HitStone);
		}
	}

	SetInteractionPromptVisible(!CollectedStones.IsEmpty());
	if (!CollectedStones.IsEmpty())
	{
		return;
	}

	// Sphere 进入事件只触发一次；仍在拾取区域时保留扫描，允许继续靠近后选中目标。
	TArray<AActor*> NearbyPickUps;
	AvatarActor->GetOverlappingActors(NearbyPickUps, AWarriorPickUpBase::StaticClass());
	if (!NearbyPickUps.ContainsByPredicate([](const AActor* PickUp) { return IsValid(PickUp); }))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}

void UHeroGameplayAbility_Interact::ConsumeStones()
{
	if (CollectedStones.IsEmpty())
	{
		return;
	}

	for (AWarriorStoneBase* Stone : CollectedStones)
	{
		if (IsValid(Stone))
		{
			Stone->Consume(GetWarriorAbilitySystemComponent(), GetAbilityLevel());
		}
	}
}
