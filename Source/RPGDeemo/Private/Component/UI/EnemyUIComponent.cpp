// MyRPGDemo


#include "Component/UI/EnemyUIComponent.h"

void UEnemyUIComponent::RegisterEnemyDrawWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawWidgetIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty())
	{
		return;
	}

	for (UWarriorWidgetBase* Widget : EnemyDrawnWidgets)
	{
		if (Widget)
		{
			Widget->RemoveFromParent();
		}
	}
	EnemyDrawnWidgets.Empty();
}
