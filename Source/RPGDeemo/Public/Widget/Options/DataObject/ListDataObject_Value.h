// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Widget/Options/OptionsDataInteractionHelper.h"
#include "Widget/Options/DataObject/ListDataObject_Base.h"
#include "ListDataObject_Value.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UListDataObject_Value : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter);
	void SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter);

	void SetDefaultValueString(const FString& InDefaultValueString){DefaultStringValue = InDefaultValueString;};

	virtual bool HasDefaultValue() const override;

protected:
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicGetter;
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicSetter;

	FString GetDefaultValueString()const{return DefaultStringValue.GetValue();};
private:
	TOptional<FString> DefaultStringValue;
};
