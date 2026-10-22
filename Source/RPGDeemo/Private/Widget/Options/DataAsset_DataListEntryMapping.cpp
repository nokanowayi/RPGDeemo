// MyRPGDemo

#include "Widget/Options/DataAsset_DataListEntryMapping.h"
#include "widget/Options/DataObject/ListDataObject_Base.h"

TSubclassOf<UWidget_ListEntry_Base> UDataAsset_DataListEntryMapping::FindEntryWidgetClassForDataObject(
	UListDataObject_Base* InDataObject) const
{
	for (UClass* DataObjectClass = InDataObject->GetClass(); DataObjectClass != nullptr; DataObjectClass = DataObjectClass->GetSuperClass())
	{
		if (const TSubclassOf<UWidget_ListEntry_Base>* FoundEntryWidgetClass = DataListEntryMap.Find(DataObjectClass))
		{
			return *FoundEntryWidgetClass;
		}
	}
	return nullptr;
}
