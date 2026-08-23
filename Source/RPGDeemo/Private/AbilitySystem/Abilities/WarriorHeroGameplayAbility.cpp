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
	UE_LOG(LogTemp, Warning, TEXT("[HUD] GetHeroController: AvatarActor=%s Controller=%s"),
		GetAvatarActorFromActorInfo() ? *GetAvatarActorFromActorInfo()->GetName() : TEXT("NULL"),
		CachedWarriorHeroController.IsValid() ? *CachedWarriorHeroController.Get()->GetName() : TEXT("NULL"));
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
	

	float AbilityLevel = GetAbilityLevel();

	FGameplayEffectContextHandle Handle = GetWarriorAbilitySystemComponent()->MakeEffectContext();
	Handle.SetAbility(this);
	Handle.AddSourceObject(GetAvatarActorFromActorInfo());
	Handle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = GetWarriorAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, AbilityLevel, Handle);
	

	SpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

	if (InCurrentAttackTypeTag.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
	}
	return SpecHandle;
}
