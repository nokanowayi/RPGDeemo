// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WarriorGameplayTags.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "Widget/Widget_ActivatableBase.h"
#include "Widget/Widget_PrimaryLayout.h"
#include "Widget/Components/FrontendCommonButtonBase.h"
#include "FrontendUISubsystem.generated.h"

enum class EAsyncPushWidgetState:uint8
{
	OnCreatedBeforePush,
	AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate,UFrontendCommonButtonBase*,InButton,FText,InDescriptionText);
UCLASS()
class RPGDEEMO_API UFrontendUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UFrontendUISubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InPrimaryLayoutWidget);

	void PushSoftWidgetToStackAsync(const FGameplayTag& InStackTag,TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,TFunction<void(EAsyncPushWidgetState,UWidget_ActivatableBase*)> AsyncPushStateCallBack);

	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InConfirmScreenType,const FText& InScreenTitle,const FText& InScreenMessage,TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	UWidget_PrimaryLayout* GetPrimaryLayoutWidget() const { return CreatedPrimaryLayoutWidget; }

	UPROPERTY(blueprintAssignable, Category = "UI")
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;

private:
	UPROPERTY(transient)
	UWidget_PrimaryLayout* CreatedPrimaryLayoutWidget = nullptr;
};
