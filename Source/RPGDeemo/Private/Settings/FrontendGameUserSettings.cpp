// MyRPGDemo


#include "Settings/FrontendGameUserSettings.h"

#include "WarriorDebugHelper.h"

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

FString UFrontendGameUserSettings::GetScreenResolutionString() const
{
	// 格式必须和 UListDataObject_StringResolution::ResToValueString 完全一致，
	// 否则动态 getter 的字符串匹配不上选项（初始化会显示 Invalid Value）
	const FIntPoint ScreenResolution = GetScreenResolution();
	return FString::Printf(TEXT("X = %i , Y = %i"), ScreenResolution.X, ScreenResolution.Y);
}

void UFrontendGameUserSettings::SetScreenResolutionString(const FString& InValue)
{
	int32 Width = 0;
	int32 Height = 0;

	// 支持 "X = 1920 , Y = 1080"，也兼容 "1920x1080"
	if (!FParse::Value(*InValue, TEXT("X"), Width) || !FParse::Value(*InValue, TEXT("Y"), Height))
	{
		FString LeftPart;
		FString RightPart;
		if (InValue.Split(TEXT("x"), &LeftPart, &RightPart))
		{
			Width = FCString::Atoi(*LeftPart.TrimStartAndEnd());
			Height = FCString::Atoi(*RightPart.TrimStartAndEnd());
		}
	}

	if (Width <= 0 || Height <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FrontendGameUserSettings] 无法解析分辨率字符串: %s"), *InValue);
		return;
	}

	SetScreenResolution(FIntPoint(Width, Height));

	// 诊断（左上角 + Output Log）
	Debug::Print(FString::Printf(TEXT("[Resolution] setter called: %dx%d  mode=%s"),
		Width, Height, *UEnum::GetValueAsString(GetFullscreenMode())),
		FColor::Yellow, 6.f, 9003);

	// 无边框全屏下窗口尺寸被强制等于桌面分辨率，改分辨率不会生效（引擎行为）
	if (GetFullscreenMode() == EWindowMode::WindowedFullscreen)
	{
		Debug::Print(TEXT("[Resolution] WindowedFullscreen 下分辨率不生效，请切 Windowed/Fullscreen"),
			FColor::Red, 6.f, 9004);
	}

	// 只写 ResolutionSizeX/Y 不会生效，必须走一次 ApplyResolutionSettings
	ApplyResolutionSettings(/*bCheckForCommandLineOverrides=*/false);
}
