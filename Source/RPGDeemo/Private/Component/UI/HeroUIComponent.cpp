// MyRPGDemo


#include "Component/UI/HeroUIComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Component/WarriorLevelComponent.h"

void UHeroUIComponent::BroadcastCurrentRageState()
{
	APawn* OwningPawn = GetOwningPawn();
	if (!OwningPawn) return;
	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningPawn))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (const UWarriorAttributeSet* AttrSet = Cast<UWarriorAttributeSet>(ASC->GetAttributeSet(UWarriorAttributeSet::StaticClass())))
			{
				const float MaxRage = AttrSet->GetMaxRage();
				const float Percent = MaxRage > 0.f ? AttrSet->GetCurrentRage() / MaxRage : 1.f;
				OnCurrentRageChanged.Broadcast(Percent);
			}
		}
	}
}

void UHeroUIComponent::BroadcastCurrentExpState()
{
	APawn* OwningPawn = GetOwningPawn();
	if (!OwningPawn) return;
	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningPawn))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (const UWarriorAttributeSet* AttrSet = Cast<UWarriorAttributeSet>(ASC->GetAttributeSet(UWarriorAttributeSet::StaticClass())))
			{
				const float CurrentExp = AttrSet->GetCurrentExp();
				const float MaxExp = Cast<AWarriorHeroCharacter>(ASC->GetOwnerActor())->GetWarriorLevelComponent()->GetExpToNextLevel(static_cast<int32>(AttrSet->GetCharacterLevel()));
				const float Percent = MaxExp > 0.f ? CurrentExp / MaxExp : 1.f;
				OnExpChanged.Broadcast(Percent);
			}
		}
	}
}

void UHeroUIComponent::BroadcastCurrentLevelState()
{
	APawn* OwningPawn = GetOwningPawn();
	if (!OwningPawn) return;
	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningPawn))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (const UWarriorAttributeSet* AttrSet = Cast<UWarriorAttributeSet>(ASC->GetAttributeSet(UWarriorAttributeSet::StaticClass())))
			{
				OnLevelUpDelegate.Broadcast(AttrSet->GetCharacterLevel());
			}
		}
	}
}
