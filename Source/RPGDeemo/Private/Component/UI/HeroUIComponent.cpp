// MyRPGDemo


#include "Component/UI/HeroUIComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/WarriorAttributeSet.h"

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
