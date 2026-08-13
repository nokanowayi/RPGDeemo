// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION(blueprintImplementableEvent,meta =(DisplayName = "On Owning Hero UI Component Initialized"))
	void BP_OnOwningHeroUIComponentInitialized(class UHeroUIComponent* OwningHeroUIComponent);

	UFUNCTION(blueprintImplementableEvent,meta =(DisplayName = "On Owning Enemy UI Component Initialized"))
	void BP_OnOwningEnemyUIComponentInitialized(class UEnemyUIComponent* OwningEnemyUIComponent);

public:
	UFUNCTION(BlueprintCallable)
	void InitEnemyWidget(AActor* OwningEnemyActor);
};
