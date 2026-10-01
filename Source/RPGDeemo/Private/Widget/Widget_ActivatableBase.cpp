// MyRPGDemo


#include "Widget/Widget_ActivatableBase.h"

AFrontendPlayerController* UWidget_ActivatableBase::GetOwningController()
{
	if (!CachedOwningController.IsValid())
	{
		CachedOwningController = GetOwningPlayer<AFrontendPlayerController>();
	}

	return CachedOwningController.IsValid()?CachedOwningController.Get():nullptr;
}

