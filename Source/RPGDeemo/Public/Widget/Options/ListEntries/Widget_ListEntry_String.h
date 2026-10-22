// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Widget/Components/FrontendCommonButtonBase.h"
#include "Widget/Components/FrontendCommonRotator.h"
#include "Widget/Options/DataObject/ListDataObject_String.h"
#include "Widget/Options/ListEntries/Widget_ListEntry_Base.h"
#include "Widget_ListEntry_String.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class RPGDEEMO_API UWidget_ListEntry_String : public UWidget_ListEntry_Base
{
	GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;
	
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;

	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,EOptionsListDataModifyReason ModifyReason) override;

private:
	
	void OnIncreaseButtonClicked();
	void OnDecreaseButtonClicked();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UFrontendCommonButtonBase* CommonButton_Decrease;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UFrontendCommonRotator* CommonRotator_AvailableOptions;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UFrontendCommonButtonBase* CommonButton_Increase;

	UPROPERTY(Transient)
	UListDataObject_String* CachedOwningStringDataObject;
};
