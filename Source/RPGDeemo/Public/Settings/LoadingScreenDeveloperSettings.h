// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LoadingScreenDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class RPGDEEMO_API ULoadingScreenDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	TSubclassOf<UUserWidget> GetLoadingScreenWidgetClass() const;
	UPROPERTY(config, EditAnywhere,Category="Loading Screen Settings")
	TSoftClassPtr<UUserWidget> LoadingScreenWidgetClass;

	UPROPERTY(config, EditAnywhere, Category = "Loading Screen Settings")
	float HoldLoadingScreenExtraTime = 2.f;

	UPROPERTY(config, EditAnywhere, Category = "Loading Screen Settings")
	bool bShouldLoadingScreenInEditor = false;
};
