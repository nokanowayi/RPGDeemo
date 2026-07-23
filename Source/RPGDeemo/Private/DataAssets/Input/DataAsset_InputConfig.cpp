// MyRPGDemo


#include "DataAssets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FWarriorInputActionConfig& Action : NativeInputActions)
	{
		if (Action.InputTag == InputTag && Action.InputAction)
		{
			return Action.InputAction;
		}
		
	}
	return nullptr;
}
