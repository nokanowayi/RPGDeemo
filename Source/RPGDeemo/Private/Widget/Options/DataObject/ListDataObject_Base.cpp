// MyRPGDemo


#include "Widget/Options/DataObject/ListDataObject_Base.h"

#include "Settings/FrontendGameUserSettings.h"

void UListDataObject_Base::InitDataObject()
{
	OnDataObjectInit();
}

void UListDataObject_Base::OnDataObjectInit()
{
	
}

void UListDataObject_Base::NotifyListDataModified(UListDataObject_Base* ModifyData,
	EOptionsListDataModifyReason ModifyReason)
{
	OnListDataModified.Broadcast(ModifyData, ModifyReason);

	if (bShouldApplyChangeImmediately)
	{
		UFrontendGameUserSettings::Get()->ApplySettings(true);
	}
}
