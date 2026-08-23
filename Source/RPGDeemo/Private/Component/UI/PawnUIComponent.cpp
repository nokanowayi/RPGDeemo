// MyRPGDemo


#include "Component/UI/PawnUIComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/WarriorAttributeSet.h"

void UPawnUIComponent::BroadcastCurrentHealthState()
{
	APawn* OwningPawn = GetOwningPawn();
	if (!OwningPawn) return;
	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningPawn))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			if (const UWarriorAttributeSet* AttrSet = Cast<UWarriorAttributeSet>(ASC->GetAttributeSet(UWarriorAttributeSet::StaticClass())))
			{
				const float MaxHealth = AttrSet->GetMaxHealth();
				const float Percent = MaxHealth > 0.f ? AttrSet->GetCurrentHealth() / MaxHealth : 1.f;
				OnCurrentHealthChanged.Broadcast(Percent);
			}
		}
	}
}
