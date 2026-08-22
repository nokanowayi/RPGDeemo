// MyRPGDemo


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"

#include "WarriorDebugHelper.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvaliableActorsToLock();
}

void UHeroGameplayAbility_TargetLock::GetAvaliableActorsToLock()
{
	TArray<FHitResult> BoxTraceHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacter(),
		GetHeroCharacter()->GetActorLocation(),
		GetHeroCharacter()->GetActorLocation() + GetAvatarActorFromActorInfo()->GetActorForwardVector() * TraceDistance,
		TraceBoxSize/2,
		GetHeroCharacter()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape?EDrawDebugTrace::Persistent:EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetHeroCharacter())
			{
				AvailableActorsToLock.AddUnique(HitActor);

				Debug::Print(FString::Printf(TEXT("Found Actor: %s"), *HitActor->GetName()), 5.f, FColor::Green);
			}
		}
	}
}
