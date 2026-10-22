// MyRPGDemo

#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem//Abilities/WarriorHeroGameplayAbility.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "Animation/AnimInstance.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityTypes.h"

#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"

float UWarriorAbilitySystemComponent::PlayMontage(UGameplayAbility* AnimatingAbility,
	FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate,
	FName StartSectionName, float StartTimeSeconds)
{
	UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] PlayMontage: %s owner=%s authority=%d avatar=%s AnimInst=%s"),
		Montage ? *Montage->GetName() : TEXT("NULL"),
		GetOwnerActor() ? *GetOwnerActor()->GetName() : TEXT("NULL"),
		GetOwnerActor() ? (GetOwnerActor()->HasAuthority() ? 1 : 0) : -1,
		GetAvatarActor() ? *GetAvatarActor()->GetName() : TEXT("NULL"),
		AbilityActorInfo.IsValid() && AbilityActorInfo->GetAnimInstance() ? *AbilityActorInfo->GetAnimInstance()->GetName() : TEXT("NULL"));
	return Super::PlayMontage(AnimatingAbility, ActivationInfo, Montage, InPlayRate, StartSectionName, StartTimeSeconds);
}

float UWarriorAbilitySystemComponent::PlayMontageSimulated(UAnimMontage* Montage, float InPlayRate,
	FName StartSectionName)
{
	UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] PlayMontageSimulated: %s owner=%s avatar=%s AnimInst=%s authority=%d"),
		Montage ? *Montage->GetName() : TEXT("NULL"),
		GetOwnerActor() ? *GetOwnerActor()->GetName() : TEXT("NULL"),
		GetAvatarActor() ? *GetAvatarActor()->GetName() : TEXT("NULL"),
		AbilityActorInfo.IsValid() && AbilityActorInfo->GetAnimInstance() ? *AbilityActorInfo->GetAnimInstance()->GetName() : TEXT("NULL"),
		GetOwnerActor() ? (GetOwnerActor()->HasAuthority() ? 1 : 0) : -1);
	const float Result = Super::PlayMontageSimulated(Montage, InPlayRate, StartSectionName);
	UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] PlayMontageSimulated result: %f"), Result);
	return Result;
}

void UWarriorAbilitySystemComponent::OnRep_ReplicatedAnimMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("[MONTAGE] OnRep_ReplicatedAnimMontage: owner=%s avatar=%s AnimInst=%s Anim=%s"),
		GetOwnerActor() ? *GetOwnerActor()->GetName() : TEXT("NULL"),
		GetAvatarActor() ? *GetAvatarActor()->GetName() : TEXT("NULL"),
		AbilityActorInfo.IsValid() && AbilityActorInfo->GetAnimInstance() ? *AbilityActorInfo->GetAnimInstance()->GetName() : TEXT("NULL"),
		RepAnimMontageInfo.Animation ? *RepAnimMontageInfo.Animation->GetName() : TEXT("NULL"));
	Super::OnRep_ReplicatedAnimMontage();
}

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (const FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;

		if (InputTag.MatchesTag(WarriorGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}

	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(FGameplayTag& InputTag)
{
	if (!InputTag.IsValid() || !InputTag.MatchesTag(WarriorGameplayTags::InputTag_MustBeHeld)) return;

	for (const FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		
	}
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& WeaponAbilities,const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities,
	int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpeHandles)
{
	if (WeaponAbilities.IsEmpty())
	{
		return;
	}
	for (const FWarriorHeroAbilitySet& HeroAbilitySet : WeaponAbilities)
	{
		if (!HeroAbilitySet.IsValid()) continue;
		FGameplayAbilitySpec AbilitySpec(HeroAbilitySet.AbilityToGrant, ApplyLevel);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(HeroAbilitySet.InputTag);
		
		OutGrantedAbilitySpeHandles.AddUnique(GiveAbility(AbilitySpec));
	}
	for (const FWarriorHeroSpecialAbilitySet& HeroAbilitySet : InSpecialWeaponAbilities)
	{
		if (!HeroAbilitySet.IsValid()) continue;
		FGameplayAbilitySpec AbilitySpec(HeroAbilitySet.AbilityToGrant, ApplyLevel);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(HeroAbilitySet.InputTag);
		
		OutGrantedAbilitySpeHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpeHandlesToRemove)
{
	if (InSpeHandlesToRemove.IsEmpty()) return;
	// ClearAbility 只能在服务器调用
	if (GetOwnerActor() && !GetOwnerActor()->HasAuthority()) return;
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpeHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	InSpeHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());
	
	const bool bAuthority = GetOwnerActor() && GetOwnerActor()->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("[TryActivateByTag] >>> tag=%s authority=%d owner=%s"),
		*AbilityTagToActivate.ToString(), bAuthority ? 1 : 0,
		GetOwnerActor() ? *GetOwnerActor()->GetName() : TEXT("NULL"));
	
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.Ability) continue;
		if (!Spec.Ability->AbilityTags.HasAll(AbilityTagToActivate.GetSingleTagContainer())) continue;

		const bool bSatisfy = Spec.Ability->DoesAbilitySatisfyTagRequirements(*this);
		UE_LOG(LogTemp, Warning, TEXT("  [TagMatch] %s satisfy=%d"),
			*Spec.Ability->GetName(), bSatisfy ? 1 : 0);
		if (UWarriorGameplayAbility* WarriorAbility = Cast<UWarriorGameplayAbility>(Spec.Ability))
		{
			WarriorAbility->DebugPrintActivationTagRequirements();
		}
		UE_LOG(LogTemp, Warning, TEXT("      AreAbilityTagsBlocked=%d"),
			AreAbilityTagsBlocked(Spec.Ability->AbilityTags) ? 1 : 0);
	}

	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),AbilitySpecs);

	

	if (AbilitySpecs.Num() != 0)
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, AbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = AbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{
			const bool bActivated = TryActivateAbility(SpecToActivate->Handle);
			UE_LOG(LogTemp, Warning, TEXT("[TryActivateByTag] TryActivateAbility(%s) => %s"),
				SpecToActivate->Ability ? *SpecToActivate->Ability->GetName() : TEXT("NULL"),
				bActivated ? TEXT("TRUE") : TEXT("FALSE"));
			return bActivated;
		}


	}
	return false;
}

