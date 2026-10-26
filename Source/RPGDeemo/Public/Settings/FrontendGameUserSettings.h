// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "FrontendGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UFrontendGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFrontendGameUserSettings();
	static UFrontendGameUserSettings* Get();

	/** Gameplay Collection Tab*/
	UFUNCTION()
	FString GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }

	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty){ CurrentGameDifficulty = InNewDifficulty; }
	/** Gameplay Collection Tab*/

	/** Audio Collection Tab*/
	UFUNCTION()
	float GetOverallVolume() const;

	UFUNCTION()
	void SetOverallVolume(const float& InOverallVolume);

	UFUNCTION()
	float GetMusicVolume() const;

	UFUNCTION()
	void SetMusicVolume(const float& InOverallVolume);

	UFUNCTION()
	float GetAllowBackGroundAudioVolume() const;

	UFUNCTION()
	void SetAllowBackGroundAudioVolume(const bool bToAllowBackGroundAudioVolume);
	/** Audio Collection Tab*/
private:
	/** Gameplay Collection Tab*/
	UPROPERTY(config)
	FString CurrentGameDifficulty;
	/** Gameplay Collection Tab*/
	
	/** Audio Collection Tab*/
	UPROPERTY(config)
	float OverallVolume;

	UPROPERTY(config)
	float MusicVolume;

	UPROPERTY(config)
	bool bAllowBackGroundAudio;
	/** Audio Collection Tab*/
};
