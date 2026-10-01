// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Controllers/FrontendPlayerController.h"
#include "Widget_ActivatableBase.generated.h"

/**
 * 
 */
UCLASS(Abstract,BlueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure)
	AFrontendPlayerController* GetOwningController();
private:
	TWeakObjectPtr<AFrontendPlayerController> CachedOwningController;
};
