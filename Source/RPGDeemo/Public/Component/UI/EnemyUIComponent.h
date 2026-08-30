// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Component/UI/PawnUIComponent.h"
#include "Widget/WarriorWidgetBase.h"
#include "EnemyUIComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(blueprintCallable)
	void RegisterEnemyDrawWidget(UWarriorWidgetBase* InWidgetToRegister);
	
	UFUNCTION(blueprintCallable)
	void RemoveEnemyDrawWidgetIfAny();
private:
	TArray<UWarriorWidgetBase*> EnemyDrawnWidgets;
};
