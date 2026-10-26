// MyRPGDemo


#include "Widget/Options/ListEntries/Widget_ListEntry_Scalar.h"

void UWidget_ListEntry_Scalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AnalogSlider_SettingValue->OnValueChanged.AddUniqueDynamic(this, &UWidget_ListEntry_Scalar::OnAnalogSliderValueChanged);
	AnalogSlider_SettingValue->OnMouseCaptureBegin.AddUniqueDynamic(this, &UWidget_ListEntry_Scalar::OnSliderMouseCaptureBegin);
}

void UWidget_ListEntry_Scalar::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	CachedOwningScalarDataObject = Cast<UListDataObject_Scalar>(InOwningListDataObject);

	CommonNumeric_SettingValue->SetNumericType(CachedOwningScalarDataObject->GetDisplayNumericType());
	CommonNumeric_SettingValue->FormattingSpecification = CachedOwningScalarDataObject->GetNumberFormattingOptions();
	CommonNumeric_SettingValue->SetCurrentValue(CachedOwningScalarDataObject->GetCurrentValue());

	AnalogSlider_SettingValue->SetMinValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetLowerBoundValue());
    AnalogSlider_SettingValue->SetMaxValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetUpperBoundValue());
	AnalogSlider_SettingValue->SetStepSize(CachedOwningScalarDataObject->GetSliderStepSize());
	AnalogSlider_SettingValue->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
}

void UWidget_ListEntry_Scalar::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningScalarDataObject)
	{
		CommonNumeric_SettingValue->SetCurrentValue(CachedOwningScalarDataObject->GetCurrentValue());
		AnalogSlider_SettingValue->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
	}
}

void UWidget_ListEntry_Scalar::OnAnalogSliderValueChanged(float Value)
{
	if (CachedOwningScalarDataObject)
	{
		CachedOwningScalarDataObject->SetCurrentValue(Value);
	}
}

void UWidget_ListEntry_Scalar::OnSliderMouseCaptureBegin()
{
	SelectThisEntryWidget();
}
