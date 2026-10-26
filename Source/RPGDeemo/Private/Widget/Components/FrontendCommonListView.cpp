// MyRPGDemo


#include "Widget/Components/FrontendCommonListView.h"

#include "Widget/Options/DataObject/ListDataObject_Base.h"
#include "Widget/Options/DataObject/ListDataObject_Collection.h"

UUserWidget& UFrontendCommonListView::OnGenerateEntryWidgetInternal(UObject* Item,
                                                                    TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}

	TSubclassOf<UWidget_ListEntry_Base> FoundWidgetClass = DataListEntryMapping->FindEntryWidgetClassForDataObject(Cast<UListDataObject_Base>(Item));
	return  GenerateTypedEntry<UWidget_ListEntry_Base>(FoundWidgetClass,OwnerTable);
}

bool UFrontendCommonListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
	return !FirstSelectedItem->IsA<UListDataObject_Collection>();
}
