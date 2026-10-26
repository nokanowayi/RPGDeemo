// MyRPGDemo


#include "Widget/Options/DataObject/ListDataObject_Scalar.h"

FCommonNumberFormattingOptions UListDataObject_Scalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	Options.MinimumIntegralDigits = 0;
	return Options;
}

FCommonNumberFormattingOptions UListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
	FCommonNumberFormattingOptions Options;
	Options.MinimumIntegralDigits = NumFracDigit;
	return Options;
}

float UListDataObject_Scalar::GetCurrentValue() const
{
	if (DataDynamicGetter)
	{
		return  FMath::GetMappedRangeValueClamped(OutPutValueRange,DisplayValueRange,StringToFloat(DataDynamicGetter->GetValueAsString()));
	}
	return 0.f;
}

void UListDataObject_Scalar::SetCurrentValue(float Value)
{
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueAsString(LexToString(FMath::GetMappedRangeValueClamped(DisplayValueRange,OutPutValueRange,Value)));
	}
	NotifyListDataModified(this);
}

bool UListDataObject_Scalar::CanResetBackToDefaultValue() const
{
	if (HasDefaultValue()&&DataDynamicGetter)
	{
		const float DefaultValue = StringToFloat(GetDefaultValueString());
		const float CurrentValue = StringToFloat(DataDynamicGetter->GetValueAsString());
		return !FMath::IsNearlyEqual(DefaultValue,CurrentValue,0.01);
	}
	return false;
}

bool UListDataObject_Scalar::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueAsString(GetDefaultValueString());
			NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);
			return true;
		}
	}
	return false;
}

float UListDataObject_Scalar::StringToFloat(const FString& Value) const
{
	float FloatValue = 0.5;
	LexFromString(FloatValue, *Value);
	return FloatValue;
}
