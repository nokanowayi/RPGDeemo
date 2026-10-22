// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "PropertyPathHelpers.h"
#include "Settings/FrontendGameUserSettings.h"

/**
 * 
 */
class RPGDEEMO_API FOptionsDataInteractionHelper
{
public:
	FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath);

	FString GetValueAsString() const;
	void SetValueAsString(const FString& InValue);

private:
	FCachedPropertyPath CachedDynamicFunctionPath;

	TWeakObjectPtr<UFrontendGameUserSettings> CachedWeakGameUserSettings;
};
