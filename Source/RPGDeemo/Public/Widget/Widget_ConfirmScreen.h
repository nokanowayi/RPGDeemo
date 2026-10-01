// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "Widget/Widget_ActivatableBase.h"
#include "Widget_ConfirmScreen.generated.h"

USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::Unknown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonText;
};

UCLASS()
class RPGDEEMO_API UConfirmScreenInfoObject : public UObject
{
	GENERATED_BODY()

public:
	static UConfirmScreenInfoObject* CreateOKScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	static UConfirmScreenInfoObject* CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	static UConfirmScreenInfoObject* CreateOKCancelScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	
	UPROPERTY(Transient)
	FText ScreenTitle;
	
	UPROPERTY(Transient)
	FText ScreenMessage;
	
	UPROPERTY(Transient)
	TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;
};
/**
 * 
 */
UCLASS(Abstract,BlueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UWidget_ConfirmScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	void InitConfirmScreen(UConfirmScreenInfoObject* InConfirmScreenInfoObject,TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback);
private:
    UPROPERTY(meta=(BindWidgetOptional))
	UCommonTextBlock* CommonTextBlock_Title;
	
	UPROPERTY(meta=(BindWidgetOptional))
    UCommonTextBlock* CommonTextBlock_Message;

	UPROPERTY(meta=(BindWidgetOptional))
	UDynamicEntryBox* DynamicEntryBox_Buttons;
};
