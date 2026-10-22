// MyRPGDemo


#include "Widget/Options/ListEntries/Widget_ListEntry_String.h"

void UWidget_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CommonButton_Increase->OnClicked().AddUObject(this,&ThisClass::OnIncreaseButtonClicked);
	CommonButton_Decrease->OnClicked().AddUObject(this,&ThisClass::OnDecreaseButtonClicked);
}

void UWidget_ListEntry_String::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedOwningStringDataObject = Cast<UListDataObject_String>(InOwningListDataObject);

	CommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());
	CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentTextValue());
}

void UWidget_ListEntry_String::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{

	if (OwningModifiedData == CachedOwningStringDataObject)
	{
		CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentTextValue());
	}
}

void UWidget_ListEntry_String::OnIncreaseButtonClicked()
{
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->AdvanceToNextOption();
	}
}

void UWidget_ListEntry_String::OnDecreaseButtonClicked()
{
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->BackToPreviousOption();
	}
}
