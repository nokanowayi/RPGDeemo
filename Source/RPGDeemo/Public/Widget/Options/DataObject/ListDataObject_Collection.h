// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Widget/Options/DataObject/ListDataObject_Base.h"
#include "ListDataObject_Collection.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UListDataObject_Collection : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void AddChildListData(UListDataObject_Base* InChildListData);
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const override { return ChildListDataArray; }
	virtual bool HasAnyChildListData() const override { return !ChildListDataArray.IsEmpty(); }
	
private:
	TArray<UListDataObject_Base*> ChildListDataArray;

};
