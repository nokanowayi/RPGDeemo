// MyRPGDemo


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGameplayTags.h"
#include "WarriorFunctionLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "WarriorTypes/WarriorEnumTypes.h"

void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo&&!Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
		}
	}
}

void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
		{
			// ClearAbility 只能在服务器调用，客户端跳过
			if (ActorInfo->AbilitySystemComponent->GetOwnerActor() &&
				ActorInfo->AbilitySystemComponent->GetOwnerActor()->HasAuthority())
			{
				ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
			}
		}
	}
}

UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponent() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

void UWarriorGameplayAbility::DebugPrintActivationTagRequirements() const
{
	UE_LOG(LogTemp, Warning, TEXT("  [GA:%s] ActivationBlockedTags=[%s] ActivationRequiredTags=[%s]"),
		*GetName(),
		*ActivationBlockedTags.ToStringSimple(),
		*ActivationRequiredTags.ToStringSimple());
	UE_LOG(LogTemp, Warning, TEXT("  [GA:%s] SourceBlockedTags=[%s] SourceRequiredTags=[%s]"),
		*GetName(),
		*SourceBlockedTags.ToStringSimple(),
		*SourceRequiredTags.ToStringSimple());
	UE_LOG(LogTemp, Warning, TEXT("  [GA:%s] TargetBlockedTags=[%s] TargetRequiredTags=[%s]"),
		*GetName(),
		*TargetBlockedTags.ToStringSimple(),
		*TargetRequiredTags.ToStringSimple());
}

UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponent() const
{
	return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyGameplaySpecEffectHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle) const
{
	UE_LOG(LogTemp, Warning, TEXT("=== NativeApplyGameplaySpecEffectHandleToTarget ==="));
	UE_LOG(LogTemp, Warning, TEXT("TargetActor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	UE_LOG(LogTemp, Warning, TEXT("Target ASC: %s"), ASC ? *ASC->GetName() : TEXT("NULL"));

	if (!InSpecHandle.Data.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("InSpecHandle.Data is INVALID (nullptr)"));
		return FActiveGameplayEffectHandle();
	}

	FGameplayEffectSpec* Spec = InSpecHandle.Data.Get();
	UE_LOG(LogTemp, Warning, TEXT("Spec: %s, Def: %s, Level: %.1f"),
		Spec ? TEXT("Valid") : TEXT("NULL"),
		Spec && Spec->Def ? *Spec->Def->GetName() : TEXT("NULL"),
		Spec ? Spec->GetLevel() : -1.f);

	FActiveGameplayEffectHandle Result = GetWarriorAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data.Get(), ASC);

	UE_LOG(LogTemp, Warning, TEXT("ApplyGameplayEffectSpecToTarget result: %s (Valid=%d)"),
		Result.WasSuccessfullyApplied() ? TEXT("SUCCESS") : TEXT("FAILED"),
		Result.IsValid() ? 1 : 0);

	return Result;
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyGameplaySpecEffectHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle,EWarriorSuccessType& SuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyGameplaySpecEffectHandleToTarget(TargetActor,InSpecHandle);
	ActiveGameplayEffectHandle.WasSuccessfullyApplied()? SuccessType = EWarriorSuccessType::Successful : SuccessType = EWarriorSuccessType::Failed;
	return ActiveGameplayEffectHandle;
}

void UWarriorGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle,
	const TArray<FHitResult>& InHitResults)
{
	if (InHitResults.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyGameplayEffectSpecHandleToHitResults: No hit results to apply effect to."));
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	for (const FHitResult& HitResult : InHitResults)
	{
		if (APawn* Pawn = Cast<APawn>(HitResult.GetActor()))
		{
			if(!UWarriorFunctionLibrary::IsTargetPawnHostile(OwnerPawn,Pawn))
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyGameplaySpecEffectHandleToTarget(Pawn, InSpecHandle);
				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwnerPawn;
					Data.Target = Pawn;
					
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, WarriorGameplayTags::Shared_Event_HitReact,Data );
				}
			}
		}
	}
}
