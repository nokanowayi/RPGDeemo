// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Widget/Options/DataObject/ListDataObject_Value.h"
#include "ListDataObject_String.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UListDataObject_String : public UListDataObject_Value
{
	GENERATED_BODY()
public:
	void AddDynamicOption(const FString& InOptionString, const FText& InOptionText);

	void AdvanceToNextOption();
	void BackToPreviousOption();
protected:
	virtual void OnDataObjectInit() override;

	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);
	virtual bool CanResetBackToDefaultValue()const override;
	virtual bool TryResetBackToDefaultValue() override;
	
	FString CurrentStringValue;
	FText  CurrentTextValue;
	TArray<FString> AvailableOptionsStringArray;
	TArray<FText> AvailableOptionsTextArray;

public:
	FORCEINLINE const TArray<FText>& GetAvailableOptionsTextArray() const { return AvailableOptionsTextArray; }
	FORCEINLINE const FText& GetCurrentTextValue() const { return CurrentTextValue; }
};
