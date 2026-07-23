// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "WarrriorInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarrriorInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject,typename CallBackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig* InputConfig,const FGameplayTag& InputTag,ETriggerEvent TriggerEvent,UserObject* Object,CallBackFunc Func);
};

template <class UserObject, typename CallBackFunc>
void UWarrriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserObject* Object, CallBackFunc Func)
{
	checkf(InputConfig,TEXT("InputConfig is nullptr, please check the input config data asset!"));
	if (UInputAction* FoundAction = InputConfig->FindInputActionByTag(InputTag))
	{
		BindAction(FoundAction,TriggerEvent,Object,Func);
	}
}
