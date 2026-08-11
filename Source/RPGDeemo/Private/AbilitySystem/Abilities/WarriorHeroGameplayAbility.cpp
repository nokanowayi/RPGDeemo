// MyRPGDemo


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

#include "WarriorGameplayTags.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacter()
{
	if (!CachedWarriorHeroCharacter.IsValid()){
		CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedWarriorHeroCharacter.IsValid()?CachedWarriorHeroCharacter.Get():nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroController()
{
	if (!CachedWarriorHeroController.IsValid())
	{
		CachedWarriorHeroController = Cast<AWarriorHeroController>(GetHeroCharacter()->GetController());
	}
	return CachedWarriorHeroController.IsValid()?CachedWarriorHeroController.Get():nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponent()
{
	return GetHeroCharacter()->GetHeroCombatComponent();
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);

	UE_LOG(LogTemp, Warning, TEXT("=== MakeHeroDamageEffectSpecHandle ==="));
	UE_LOG(LogTemp, Warning, TEXT("EffectClass: %s, WeaponBaseDamage: %.1f, AttackTypeTag: %s, UsedComboCount: %d"),
		*EffectClass->GetName(), InWeaponBaseDamage, *InCurrentAttackTypeTag.ToString(), InUsedComboCount);

	float AbilityLevel = GetAbilityLevel();
	UE_LOG(LogTemp, Warning, TEXT("AbilityLevel: %.1f, IsInstantiated: %d, IsActive: %d"),
		AbilityLevel, IsInstantiated() ? 1 : 0, IsActive() ? 1 : 0);

	FGameplayEffectContextHandle Handle = GetWarriorAbilitySystemComponent()->MakeEffectContext();
	Handle.SetAbility(this);
	Handle.AddSourceObject(GetAvatarActorFromActorInfo());
	Handle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = GetWarriorAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, AbilityLevel, Handle);

	UE_LOG(LogTemp, Warning, TEXT("SpecHandle valid: %d, Data valid: %d, Def: %s"),
		SpecHandle.IsValid() ? 1 : 0,
		SpecHandle.Data.IsValid() ? 1 : 0,
		(SpecHandle.Data.IsValid() && SpecHandle.Data->Def) ? *SpecHandle.Data->Def->GetName() : TEXT("NULL"));

	SpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

	if (InCurrentAttackTypeTag.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
	}
	return SpecHandle;
}
