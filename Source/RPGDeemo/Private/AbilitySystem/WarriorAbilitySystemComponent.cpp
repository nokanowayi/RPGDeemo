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
	// ClearAbility 只能在服务器调用（引擎限制），客户端跳过
	if (GetOwnerActor() && !GetOwnerActor()->HasAuthority()) return;
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpeHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	// 循环结束后统一清空，避免迭代器失效
	InSpeHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	// ===== 诊断日志：定位 SpawnStone 不触发的断点 =====
	const bool bAuthority = GetOwnerActor() && GetOwnerActor()->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("[TryActivateByTag] >>> tag=%s authority=%d owner=%s"),
		*AbilityTagToActivate.ToString(), bAuthority ? 1 : 0,
		GetOwnerActor() ? *GetOwnerActor()->GetName() : TEXT("NULL"));

	// 打印当前 ASC 上所有仍被授予的能力（排查：没授予 / 被 OnGiven 的 ClearAbility 移除 / Ability Tags 配错）
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		UE_LOG(LogTemp, Warning, TEXT("  [Granted] %s active=%d tags=[%s]"),
			Spec.Ability ? *Spec.Ability->GetName() : TEXT("NULL"),
			Spec.IsActive() ? 1 : 0,
			Spec.Ability ? *Spec.Ability->AbilityTags.ToStringSimple() : TEXT(""));
	}

	// ===== 诊断日志2：定位 tag 命中但 GetActivatableGameplayAbilitySpecsByAllMatchingTags 仍 matched=0 的原因 =====
	// 该函数内部还会调用 DoesAbilitySatisfyTagRequirements（检查 Blocked/Required Tag 条件），这里手动复算并打印细节
	UE_LOG(LogTemp, Warning, TEXT("  [OwnedTags] %s"), *GetOwnedGameplayTags().ToStringSimple());
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

	UE_LOG(LogTemp, Warning, TEXT("[TryActivateByTag] matched=%d"), AbilitySpecs.Num());
	for (FGameplayAbilitySpec* Spec : AbilitySpecs)
	{
		UE_LOG(LogTemp, Warning, TEXT("  [Matched] %s active=%d"),
			Spec && Spec->Ability ? *Spec->Ability->GetName() : TEXT("NULL"),
			Spec ? (Spec->IsActive() ? 1 : 0) : -1);
	}

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

