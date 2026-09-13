// MyRPGDemo


#include "Component/WarriorLevelComponent.h"

#include "WarriorDebugHelper.h"
#include "AbilitySystem/WarriorAttributeSet.h" 
#include "Characters/WarriorHeroCharacter.h"

// Sets default values for this component's properties
UWarriorLevelComponent::UWarriorLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UWarriorLevelComponent::GainExp(int32 InExp)
{
	SettleExpInternal(InExp);
}

void UWarriorLevelComponent::SettleExp()
{
	SettleExpInternal(0.f);
}

void UWarriorLevelComponent::InitializeStats()
{
	ApplyLevelGrowth(1);
	AWarriorHeroCharacter* WarriorHeroCharacter = Cast<AWarriorHeroCharacter>(GetOwner());
	if (!WarriorHeroCharacter) return;
	if (UHeroUIComponent* UIComponent = WarriorHeroCharacter->GetHeroUIComponent())
	{
		UIComponent->OnExpChanged.Broadcast(0.f);
	}
	UWarriorAttributeSet* AttrSet = WarriorHeroCharacter->GetWarriorAttributeSet();
	if (!AttrSet) return;
	
	if (AttrSet->GetCharacterLevel() < 1.f)
	{
		AttrSet->SetCharacterLevel(1.f);
	}
}

void UWarriorLevelComponent::SettleExpInternal(int32 InExp)
{
	AWarriorHeroCharacter* WarriorHeroCharacter = Cast<AWarriorHeroCharacter>(GetOwner());
	if (!WarriorHeroCharacter) return;
	
	UWarriorAbilitySystemComponent* ASC = WarriorHeroCharacter->GetWarriorAbilitySystemComponent();
	if (!ASC->IsOwnerActorAuthoritative()) return;
	
	UWarriorAttributeSet* AttrSet = WarriorHeroCharacter->GetWarriorAttributeSet();
	UHeroUIComponent* UIComponent = WarriorHeroCharacter->GetHeroUIComponent();
	if (!AttrSet) return;

	if (!ASC->IsOwnerActorAuthoritative()) return;

	
	float RemainingExp = FMath::Max(0.f, AttrSet->GetCurrentExp()) + static_cast<float>(InExp);
	int32 CurrentLevel = AttrSet->GetCharacterLevel();
	const int32 OldLevel = CurrentLevel;

	Debug::Print(FString::Printf(TEXT("[Level] %s: CurrentLevel=%d"), *GetName(), CurrentLevel), FColor::Green, 5.f);
	int32 Exp = GetExpToNextLevel(CurrentLevel);
	while (RemainingExp >= Exp && Exp > 0.f && CurrentLevel < MaxLevel)
	{
		RemainingExp -= ExpToNextLevel.GetValueAtLevel(CurrentLevel);
		++CurrentLevel;
		Exp = ExpToNextLevel.GetValueAtLevel(CurrentLevel);
	}

	
	AttrSet->SetCurrentExp(RemainingExp);

	if (CurrentLevel != OldLevel)
	{
		AttrSet->SetCharacterLevel(static_cast<float>(CurrentLevel));
	}

	UIComponent->OnLevelUpDelegate.Broadcast(CurrentLevel);
	if (!UIComponent) return;
	if (CurrentLevel < MaxLevel)
	{
		UIComponent->OnExpChanged.Broadcast(static_cast<float>(RemainingExp) / static_cast<float>(Exp));
	}
	else
	{
		UIComponent->OnExpChanged.Broadcast(1.f);
	}
}

void UWarriorLevelComponent::ApplyLevelGrowth(int32 NewLevel)
{
	AWarriorHeroCharacter* Hero = Cast<AWarriorHeroCharacter>(GetOwner());
	if (!Hero) return;

	UWarriorAbilitySystemComponent* ASC = Hero->GetWarriorAbilitySystemComponent();
	if (!ASC) return;
	if (!ASC->IsOwnerActorAuthoritative()) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	if (LevelUpStatsEffect)
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(LevelUpStatsEffect, NewLevel, Context);
		if (Spec.IsValid()) ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
	
	if (FillStatsEffect)
	{
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(FillStatsEffect, 1, Context);
		if (Spec.IsValid()) ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
	AWarriorHeroCharacter* WarriorHeroCharacter = Cast<AWarriorHeroCharacter>(GetOwner());
	if (!WarriorHeroCharacter) return;
	
	UWarriorAttributeSet* AttrSet = WarriorHeroCharacter->GetWarriorAttributeSet();
}



