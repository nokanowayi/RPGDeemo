// MyRPGDemo


#include "Widget/Options/DataObject/ListDataObject_Base.h"

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
}
