// MyRPGDemo


#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"

UWarriorAttributeSet::UWarriorAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.F);
	InitDefensePower(1.f);
}

void UWarriorAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface.IsValid() ? CachedPawnUIInterface->GetPawnUIComponent() : nullptr;
	
	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		const float NewMaxHealth = FMath::Max(1.f, GetMaxHealth());
		SetMaxHealth(NewMaxHealth);

		const float CurrentHealthValue = GetCurrentHealth();
		if (FMath::IsNearlyEqual(CurrentHealthValue, 1.f) && NewMaxHealth > 1.f)
		{
			SetCurrentHealth(NewMaxHealth);
		}
		else
		{
			SetCurrentHealth(FMath::Clamp(CurrentHealthValue, 0.f, NewMaxHealth));
		}

		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());
		
		SetCurrentHealth(NewCurrentHealth);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(NewCurrentHealth);
	}
	
	if (Data.EvaluatedData.Attribute == GetMaxRageAttribute())
	{
		const float NewMaxRage = FMath::Max(1.f, GetMaxRage());
		SetMaxRage(NewMaxRage);

		const float CurrentRageValue = GetCurrentRage();
		if (FMath::IsNearlyEqual(CurrentRageValue, 1.f) && NewMaxRage > 1.f)
		{
			SetCurrentRage(NewMaxRage);
		}
		else
		{
			SetCurrentRage(FMath::Clamp(CurrentRageValue, 0.f, NewMaxRage));
		}

		if (CachedPawnUIInterface->GetHeroUIComponent()) CachedPawnUIInterface->GetHeroUIComponent()->OnCurrentRageChanged.Broadcast(GetCurrentRage() / GetMaxRage());
	}
	
	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());
		
		SetCurrentRage(NewCurrentRage);

		if (CachedPawnUIInterface->GetHeroUIComponent()) CachedPawnUIInterface->GetHeroUIComponent()->OnCurrentRageChanged.Broadcast(GetCurrentRage() / GetMaxRage());
	}
	
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float DamageToApply = FMath::Max(0.f, GetDamageTaken());
		const float OldCurrentHealth = GetCurrentHealth();

		const float NewCurrentHealth = FMath::Clamp(OldCurrentHealth - DamageToApply, 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		Debug::Print(TEXT("NewCurrentHealth"), NewCurrentHealth, FColor::Green, 2.f);
		
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
		
		if (NewCurrentHealth == 0.f){
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Shared_Status_Dead);
		}
	}
	
}
