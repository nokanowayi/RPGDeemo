// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "Widget/Widget_ActivatableBase.h"
#include "FrontendDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game,DefaultConfig,meta = (DisplayName = "Frontend UI Settings"))
class RPGDEEMO_API UFrontendDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference",meta = (ForceInLineRow,Categories = "UI.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> FrontendWidgetMap;

	UPROPERTY(Config, EditAnywhere, Category = "Widget Image Reference",meta = (ForceInLineRow,Categories = "UI.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;
};
