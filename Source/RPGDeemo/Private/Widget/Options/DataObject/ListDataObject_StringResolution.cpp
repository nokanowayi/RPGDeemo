// MyRPGDemo


#include "Widget/Options/DataObject/ListDataObject_StringResolution.h"

#include "Kismet/KismetSystemLibrary.h"
#include "WarriorDebugHelper.h"

void UListDataObject_StringResolution::InitResolutionValues()
{
	TArray<FIntPoint> Resolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);

	Resolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return A.X * A.Y > B.X * B.Y;
	});
	
	for (const FIntPoint& Resolution : Resolutions)
	{
		AddDynamicOption(ResToValueString(Resolution), ResToDisplayText(Resolution));
	}

	// 降序排序后第一个才是最大分辨率（原来取 Last() 取到了最小值）
	if (!Resolutions.IsEmpty())
	{
		MaximumAllowedResolution = ResToValueString(Resolutions[0]);
		SetDefaultValueString(MaximumAllowedResolution);
	}
}

void UListDataObject_StringResolution::OnDataObjectInit()
{
	Super::OnDataObjectInit();

	// 动态 getter 返回的字符串匹配不上选项时，用当前屏幕分辨率兜底，并刷新显示文本
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentStringValue = ResToValueString(UFrontendGameUserSettings::Get()->GetScreenResolution());
		TrySetDisplayTextFromStringValue(CurrentStringValue);
	}

	// 诊断：初始化时到底拿到了什么值、选项有多少个
	Debug::Print(FString::Printf(TEXT("[Resolution] init = %s  (options: %d)"),
		*CurrentStringValue, AvailableOptionsStringArray.Num()),
		FColor::Cyan, 6.f, 9002);
}

FString UListDataObject_StringResolution::ResToValueString(const FIntPoint& InResolution) const
{
	return FString::Printf(TEXT("X = %i , Y = %i"),InResolution.X,InResolution.Y);
}

FText UListDataObject_StringResolution::ResToDisplayText(const FIntPoint& InResolution) const
{
	const FString DisString = FString::Printf(TEXT("%i x %i"), InResolution.X, InResolution.Y);
	return FText::FromString(DisString);
}
