// MyRPGDemo


#include "AbilitySystem/Abilities/HeroGameplayAbility_Interact.h"

#include "WarriorDebugHelper.h"
#include "Component/UI/HeroUIComponent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Items/PickUps/WarriorDoorBase.h"
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
}

void UHeroGameplayAbility_Interact::SetVis()
{
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

	AWarriorPickUpBase* PickUp = nullptr;
	
	for (const FHitResult& Hit : TraceHit)
	{
		if (AWarriorPickUpBase* HitPickUp =
			Cast<AWarriorPickUpBase>(Hit.GetActor()); IsValid(HitPickUp))
		{
			PickUp = HitPickUp;
			break;
		}
	}

	SetInteractionPromptVisible(IsValid(PickUp));
	TArray<AActor*> NearbyPickUps;
	AvatarActor->GetOverlappingActors(NearbyPickUps, AWarriorPickUpBase::StaticClass());
	if (!NearbyPickUps.ContainsByPredicate([](const AActor* PickUp) { return IsValid(PickUp); }))
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}

void UHeroGameplayAbility_Interact::InteractDoor()
{
	if (!GetCurrentActorInfo() || !GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}
	
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
		if (AWarriorDoorBase* HitDoor =
			Cast<AWarriorDoorBase>(Hit.GetActor()); IsValid(HitDoor))
		{
			Debug::Print(TEXT("Interacting with door"));
			HitDoor->Interact("");
			break;
		}
	}
	
}

void UHeroGameplayAbility_Interact::ConsumeStones()
{
	if (!GetCurrentActorInfo() || !GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}
	
	CollectStones();
	if (!IsActive() || CollectedStones.IsEmpty())
	{
		return;
	}
	
	const TArray<AWarriorStoneBase*> StonesToConsume = CollectedStones;
	for (AWarriorStoneBase* Stone : StonesToConsume)
	{
		if (IsValid(Stone))
		{
			Stone->Consume(GetWarriorAbilitySystemComponent(), GetAbilityLevel());
		}
	}
}
