// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "Widget/Options/DataAsset_DataListEntryMapping.h"
#include "FrontendCommonListView.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UFrontendCommonListView : public UCommonListView
{
	GENERATED_BODY()

protected:
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;

private:
	UPROPERTY(EditAnywhere, Category = "Frontend Common List View")
	UDataAsset_DataListEntryMapping* DataListEntryMapping;
};
