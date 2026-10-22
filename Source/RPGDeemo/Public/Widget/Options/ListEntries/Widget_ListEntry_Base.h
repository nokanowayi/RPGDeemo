// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widget/Options/DataObject/ListDataObject_Base.h"
#include "Widget_ListEntry_Base.generated.h"

/**
 * 
 */
UCLASS(Abstract,blueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UWidget_ListEntry_Base : public UCommonUserWidget,public IUserObjectListEntry
{
	GENERATED_BODY()
	public:
	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName = "OnListEntryWidgetHovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered,bool bIsEntryWidgetStillSelected);
	void NativeOnListEntryWidgetHovered(bool bWasHovered);

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject);

	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,EOptionsListDataModifyReason ModifyReason);
	
	void SelectThisEntryWidget();
private:
	UPROPERTY(BlueprintReadOnly,meta = (BindWidgetOptional,AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_SettingDisplayName;
};
