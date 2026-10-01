// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ListDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType,PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const { return PropertyName; } \
	void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName; }
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()
public:
	LIST_DATA_ACCESSOR(FName, DataID)
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)
	LIST_DATA_ACCESSOR(FText, DisplayName)
	LIST_DATA_ACCESSOR(FText, DisabledRichText)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
	LIST_DATA_ACCESSOR(UListDataObject_Base*, ParentData)

	void InitDataObject();

	virtual TArray<UListDataObject_Base*> GetAllChildListData() const{ return TArray<UListDataObject_Base*>(); }
	virtual bool HasAnyChildListData() const { return false; }

protected:
	virtual void OnDataObjectInit();
	
private:
	FName DataID;
	FText DescriptionRichText;
	FText DisplayName;
	FText DisabledRichText;
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	UPROPERTY(Transient)
	UListDataObject_Base* ParentData;
};
