// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "AsyncAction_PushConfirmScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushConfirmScreenButtonClickedDelegate, EConfirmScreenButtonType, ClickedButtonType);
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UAsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject",BlueprintInternalUseOnly = "true" ,DisplayName = "Show Confirmation To Screen"), Category = "UI")
	static UAsyncAction_PushConfirmScreen* PushConfirmScreenToModalStackAsync(UObject* WorldContextObject,
		EConfirmScreenType InConfirmScreenType,
		const FText InScreenTitle, const FText InScreenMessage);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FOnPushConfirmScreenButtonClickedDelegate OnButtonClicked;
private:
	TWeakObjectPtr<UWorld> CachedOwingWorld;
	EConfirmScreenType CachedConfirmScreenType;
	FText CachedScreenTitle;
	FText CachedScreenMessage;
};
