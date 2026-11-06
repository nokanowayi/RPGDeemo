// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Widget/Options/DataObject/ListDataObject_String.h"
#include "ListDataObject_StringResolution.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UListDataObject_StringResolution : public UListDataObject_String
{
	GENERATED_BODY()

public:
	void InitResolutionValues();

protected:
	virtual void OnDataObjectInit() override;

private:
	FString ResToValueString(const FIntPoint& InResolution)const;
	FText ResToDisplayText(const FIntPoint& InResolution)const;

	FString MaximumAllowedResolution;
};
