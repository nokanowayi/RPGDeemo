// MyRPGDemo


#include "Settings/FrontendGameUserSettings.h"

UFrontendGameUserSettings::UFrontendGameUserSettings()
	: OverallVolume(1.f),MusicVolume(1.f),bAllowBackGroundAudio(false)
{
	
}

UFrontendGameUserSettings* UFrontendGameUserSettings::Get()
{
	if (GEngine)
	{
		 return CastChecked<UFrontendGameUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}

float UFrontendGameUserSettings::GetOverallVolume() const
{
	//todo:修改声音
	return OverallVolume;
}

void UFrontendGameUserSettings::SetOverallVolume(const float& InOverallVolume)
{
	OverallVolume = InOverallVolume;
}

float UFrontendGameUserSettings::GetMusicVolume() const
{
	return MusicVolume;
}

void UFrontendGameUserSettings::SetMusicVolume(const float& InOverallVolume)
{
	MusicVolume = InOverallVolume;
}

float UFrontendGameUserSettings::GetAllowBackGroundAudioVolume() const
{
	return bAllowBackGroundAudio;
}

void UFrontendGameUserSettings::SetAllowBackGroundAudioVolume(const bool bToAllowBackGroundAudioVolume)
{
	bAllowBackGroundAudio = bToAllowBackGroundAudioVolume;
}
