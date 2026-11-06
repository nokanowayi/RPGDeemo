// MyRPGDemo


#include "Widget/Options/DataObject/ListDataObject_String.h"

#include "WarriorDebugHelper.h"

void UListDataObject_String::OnDataObjectInit()
{
	if (!AvailableOptionsTextArray.IsEmpty())
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultValueString();
	}
	
	if (DataDynamicGetter)
	{
		if (DataDynamicGetter.IsValid())
		{
			CurrentStringValue = DataDynamicGetter->GetValueAsString();
		}

	}
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentTextValue = FText::FromString("Invalid Value");
	}
}

void UListDataObject_String::AddDynamicOption(const FString& InOptionString, const FText& InOptionText)
{
	AvailableOptionsStringArray.Add(InOptionString);
	AvailableOptionsTextArray.Add(InOptionText);
}

void UListDataObject_String::AdvanceToNextOption()
{
	if (AvailableOptionsStringArray.IsEmpty()|| AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}

	if (AvailableOptionsStringArray.IsValidIndex(AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue) + 1))
	{
		CurrentStringValue = AvailableOptionsStringArray[AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue) + 1];
	}
	else
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	// 诊断：Rotator 是否走到这里、值变成了什么、setter 是否有效
	Debug::Print(FString::Printf(TEXT("[String] %s -> %s  (setter valid: %d, %s)"),
		*GetDataID().ToString(), *CurrentStringValue,
		DataDynamicSetter.IsValid() ? 1 : 0,
		DataDynamicSetter.IsValid() ? TEXT("will call") : TEXT("SKIPPED")),
		FColor::Cyan, 6.f, 9001);

	if (DataDynamicSetter.IsValid())
	{
		DataDynamicSetter->SetValueAsString(CurrentStringValue);

		Debug::Print(FString::Printf(TEXT("DataSetter Used") ), 5.f, FColor::Green);
		NotifyListDataModified(this);
	}
}

void UListDataObject_String::BackToPreviousOption()
{
	if (AvailableOptionsStringArray.IsEmpty()|| AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}

	if (AvailableOptionsStringArray.IsValidIndex(AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue) -1))
	{
		CurrentStringValue = AvailableOptionsStringArray[AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue) -1];
	}
	else
	{
		Debug::Print(FString::Printf(TEXT("DataSetter Used")), 5.f, FColor::Green);
		CurrentStringValue = AvailableOptionsStringArray[AvailableOptionsStringArray.Num() - 1];
	}

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (DataDynamicSetter.IsValid())
	{
		DataDynamicSetter->SetValueAsString(CurrentStringValue);
		NotifyListDataModified(this);
	}
}



bool UListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	int32 Index = AvailableOptionsStringArray.IndexOfByKey(InStringValue);
	if (AvailableOptionsTextArray.IsValidIndex(Index))
	{
		CurrentTextValue = AvailableOptionsTextArray[Index];
		return true;
	}

	return  false;
}

bool UListDataObject_String::CanResetBackToDefaultValue() const
{
	return HasDefaultValue() && CurrentStringValue != GetDefaultValueString();
}

bool UListDataObject_String::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		CurrentStringValue = GetDefaultValueString();
		TrySetDisplayTextFromStringValue(CurrentStringValue);

		if (DataDynamicSetter.IsValid())
		{
			DataDynamicSetter->SetValueAsString(CurrentStringValue);
			NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);
		}
		return true;
	}
	return false;
}

void UListDataObject_StringBool::OnDataObjectInit()
{
	TryInitBoolValues();
	Super::OnDataObjectInit();

	// getter 可能返回数值字符串（float 的 0/1），统一归一化成 True/False，
	// 否则匹配不上选项，会显示成 Invalid Value
	if (!CurrentStringValue.Equals(TrueString, ESearchCase::IgnoreCase) &&
		!CurrentStringValue.Equals(FalseString, ESearchCase::IgnoreCase))
	{
		const bool bIsTrue = CurrentStringValue.Equals(TEXT("1"), ESearchCase::IgnoreCase)
			|| CurrentStringValue.Equals(TEXT("1.0"), ESearchCase::IgnoreCase)
			|| CurrentStringValue.Equals(TEXT("1.0f"), ESearchCase::IgnoreCase);

		CurrentStringValue = bIsTrue ? TrueString : FalseString;
		TrySetDisplayTextFromStringValue(CurrentStringValue);
	}
}

void UListDataObject_StringBool::SetTrueAsDefaultValue()
{
	SetDefaultValueString(TrueString);
}

void UListDataObject_StringBool::SetFalseAsDefaultValue()
{
	SetDefaultValueString(FalseString);
}

//UListDataObject_StringBool
void UListDataObject_StringBool::TryInitBoolValues()
{
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString, FText::FromString(TEXT("On")));
	}
    if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString, FText::FromString(TEXT("Off")));
	}
}
