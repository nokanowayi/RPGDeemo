// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ListEntries/Widget_ListEntry_Base.h"
#include "DataAsset_DataListEntryMapping.generated.h"

class UListDataObject_Base;
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UDataAsset_DataListEntryMapping : public UDataAsset
{
	GENERATED_BODY()
public:
	TSubclassOf<UWidget_ListEntry_Base> FindEntryWidgetClassForDataObject(UListDataObject_Base* InDataObject) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UListDataObject_Base>, TSubclassOf<UWidget_ListEntry_Base>> DataListEntryMap;
};
