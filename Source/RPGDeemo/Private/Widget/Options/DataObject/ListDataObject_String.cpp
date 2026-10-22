// MyRPGDemo


#include "Widget/Options/DataObject/ListDataObject_String.h"

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
	NotifyListDataModified(this);
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
		CurrentStringValue = AvailableOptionsStringArray[AvailableOptionsStringArray.Num() - 1];
	}

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	NotifyListDataModified(this);
}

void UListDataObject_String::OnDataObjectInit()
{
	if (!AvailableOptionsTextArray.IsEmpty())
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentTextValue = FText::FromString("Invalid Value");
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
