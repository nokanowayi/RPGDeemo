// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widget_PrimaryLayout.generated.h"

/**
 * 
 */
UCLASS(Abstract,BlueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UWidget_PrimaryLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InStackTag) const;
	
protected:
	UFUNCTION(blueprintCallable)
	void RegisterWidgetStack(UPARAM(meta = (Categories = "UI.WidgetStack")) FGameplayTag InStackTag,UCommonActivatableWidgetContainerBase* InStack);
	
private:
	UPROPERTY(transient)
	TMap<FGameplayTag,UCommonActivatableWidgetContainerBase*> RegisteredWidgetStackMap;

	
};
