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

	template<class UserObject,typename CallBackFunc>
	void BindAbilityInputAction(const UDataAsset_InputConfig* InputConfig,UserObject* Object,CallBackFunc InputPressedFunc,CallBackFunc InputReleasedFunc);
};

template <class UserObject, typename CallBackFunc>
void UWarrriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserObject* Object, CallBackFunc Func)
{
	checkf(InputConfig,TEXT("Native inputConfig is nullptr, please check the input config data asset!"));
	if (UInputAction* FoundAction = InputConfig->FindInputActionByTag(InputTag))
	{
		BindAction(FoundAction,TriggerEvent,Object,Func);
	}
}

template <class UserObject, typename CallBackFunc>
void UWarrriorInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InputConfig, UserObject* Object,
	CallBackFunc InputPressedFunc, CallBackFunc InputReleasedFunc)
{
	checkf(InputConfig,TEXT("Ability inputConfig is nullptr, please check the input config data asset!"));
	for (const FWarriorInputActionConfig& InputActionConfig : InputConfig->AbilityInputActions)
	{
		if (!InputActionConfig.IsValid()) continue;
		BindAction(InputActionConfig.InputAction,ETriggerEvent::Started,Object,InputPressedFunc,InputActionConfig.InputTag);
		BindAction(InputActionConfig.InputAction,ETriggerEvent::Completed,Object,InputReleasedFunc,InputActionConfig.InputTag);
	}
}
