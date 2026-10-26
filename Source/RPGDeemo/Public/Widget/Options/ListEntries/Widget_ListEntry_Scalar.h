// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Widget/Options/ListEntries/Widget_ListEntry_Base.h"
#include "CommonNumericTextBlock.h"
#include "AnalogSlider.h"
#include "Widget/Options/DataObject/ListDataObject_Scalar.h"
#include "Widget_ListEntry_Scalar.generated.h"


/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class RPGDEEMO_API UWidget_ListEntry_Scalar : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;

private:
	UFUNCTION()
	void OnAnalogSliderValueChanged(float Value);
	UFUNCTION()
	void OnSliderMouseCaptureBegin();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonNumericTextBlock* CommonNumeric_SettingValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnalogSlider* AnalogSlider_SettingValue;
	UPROPERTY(transient)
	UListDataObject_Scalar* CachedOwningScalarDataObject;
};
