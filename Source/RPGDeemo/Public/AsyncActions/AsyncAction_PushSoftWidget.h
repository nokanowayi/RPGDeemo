// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Widget/Widget_ActivatableBase.h"
#include "AsyncAction_PushSoftWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushSoftWidgetDelegate, UWidget_ActivatableBase*, PushedWidget);
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UAsyncAction_PushSoftWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable,meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject",BlueprintInternalUseOnly = "true" ,DisplayName = "Push Soft Widget To Widget Stack"), Category = "UI")
	static UAsyncAction_PushSoftWidget* PushSoftWidgetToStackAsync(UObject* WorldContextObject,
		UPARAM(meta = (Categories = "UI.WidgetStack"))FGameplayTag InStackTag,
		APlayerController* InPlayerController,
		TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
		bool bFocusOnNewlyPushedWidget = true);

	virtual void Activate() override;
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnWidgetCreatedBeforePush;

	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnWidgetCreatedAfterPush;

private:
	TWeakObjectPtr<UWorld> CachedOwingWorld;
	TWeakObjectPtr<APlayerController> CachedOwingPC;
	TSoftClassPtr<UWidget_ActivatableBase> CachedSoftWidgetClass;
	FGameplayTag CachedStackTag;
	bool bCachedFocusOnNewlyPushedWidget = false;
};
