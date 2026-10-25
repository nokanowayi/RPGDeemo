// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "ListDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType,PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const { return PropertyName; } \
	void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName; }
/**
 * 
 */
UCLASS(Abstract)
class RPGDEEMO_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModified, UListDataObject_Base*, EOptionsListDataModifyReason);
	FOnListDataModified OnListDataModified;
	LIST_DATA_ACCESSOR(FName, DataID)
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)
	LIST_DATA_ACCESSOR(FText, DisplayName)
	LIST_DATA_ACCESSOR(FText, DisabledRichText)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
	LIST_DATA_ACCESSOR(UListDataObject_Base*, ParentData)

	void InitDataObject();

	virtual TArray<UListDataObject_Base*> GetAllChildListData() const{ return TArray<UListDataObject_Base*>(); }
	virtual bool HasAnyChildListData() const { return false; }

	void SetShouldApplySettingsImmediately(bool bShouldApply) { bShouldApplyChangeImmediately = bShouldApply; }
	virtual bool HasDefaultValue()const {return false;};
	virtual bool CanResetBackToDefaultValue()const {return false;};
	virtual bool TryResetBackToDefaultValue(){return false;};
protected:
	virtual void OnDataObjectInit();

	virtual void NotifyListDataModified(UListDataObject_Base* ModifyData,EOptionsListDataModifyReason ModifyReason = EOptionsListDataModifyReason::DirectlyModified);
private:
	FName DataID;
	FText DescriptionRichText;
	FText DisplayName;
	FText DisabledRichText;
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	UPROPERTY(Transient)
	UListDataObject_Base* ParentData;

	bool bShouldApplyChangeImmediately = false;
};
