// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Widget/Options/DataObject/ListDataObject_Value.h"
#include "CommonNumericTextBlock.h"
#include "ListDataObject_Scalar.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UListDataObject_Scalar : public UListDataObject_Value
{
	GENERATED_BODY()
	public:
	LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange);
	LIST_DATA_ACCESSOR(TRange<float>, OutPutValueRange);
	LIST_DATA_ACCESSOR(float, SliderStepSize);
	LIST_DATA_ACCESSOR(ECommonNumericType, DisplayNumericType);
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, NumberFormattingOptions);

	static FCommonNumberFormattingOptions NoDecimal();
	static FCommonNumberFormattingOptions WithDecimal(int32 NumFracDigit);

	float GetCurrentValue() const;

	void SetCurrentValue(float Value);
private:
	virtual bool CanResetBackToDefaultValue()const override;
	virtual bool TryResetBackToDefaultValue() override;
	
	float StringToFloat(const FString& Value) const;
	TRange<float> DisplayValueRange = TRange<float>(0.f, 1.f);
	TRange<float> OutPutValueRange = TRange<float>(0.f, 1.f);
	float SliderStepSize = 0.1f;
	ECommonNumericType DisplayNumericType = ECommonNumericType::Number;
	FCommonNumberFormattingOptions NumberFormattingOptions;
};
