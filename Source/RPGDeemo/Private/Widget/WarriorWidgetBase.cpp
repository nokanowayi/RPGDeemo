// MyRPGDemo


#include "Widget/WarriorWidgetBase.h"

#include "Component/Combat/EnemyCombatComponent.h"
#include "Interfaces/PawnUIInterface.h"
#include "TimerManager.h"

void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (!TryBindOwningPawnUI())
	{
		if (BindRetryCount < 10)
		{
			BindRetryCount++;
			FTimerHandle RetryTimer;
			GetWorld()->GetTimerManager().SetTimer(
				RetryTimer,
				this,
				&UWarriorWidgetBase::RetryBindOwningPawnUI,
				0.2f,
				false
			);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[HUD] 绑定重试超时，放弃"));
		}
	}
}

void UWarriorWidgetBase::RetryBindOwningPawnUI()
{
	if (!TryBindOwningPawnUI())
	{
		if (BindRetryCount < 10)
		{
			BindRetryCount++;
			FTimerHandle RetryTimer;
			GetWorld()->GetTimerManager().SetTimer(
				RetryTimer,
				this,
				&UWarriorWidgetBase::RetryBindOwningPawnUI,
				0.2f,
				false
			);
		}
	}
}

bool UWarriorWidgetBase::TryBindOwningPawnUI()
{
	APlayerController* PC = GetOwningPlayer();
	APawn* OwningPawn = PC ? PC->GetPawn() : nullptr;

	if (OwningPawn)
	{
		if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningPawn))
		{
			if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
			{
				UE_LOG(LogTemp, Warning, TEXT("[HUD] 绑定成功: %s"), *HeroUIComponent->GetName());
				BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
				
				HeroUIComponent->BroadcastCurrentHealthState();
				HeroUIComponent->BroadcastCurrentRageState();
				return true;
			}
		}
	}
	return false;
}

void UWarriorWidgetBase::InitEnemyWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		if (UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent())
		{
			BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
		}
	}
}
