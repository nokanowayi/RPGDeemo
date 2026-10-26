// MyRPGDemo


#include "Widget/Options/OptionsDataRegistry.h"

#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Widget/Options/DataObject/ListDataObject_String.h"
#include "Widget/Options/DataObject/ListDataObject_Collection.h"
#include "Widget/Options/DataObject/ListDataObject_Scalar.h"

class UFrontendGameUserSettings;
#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, SetterOrGetterFuncName))

void UOptionsDataRegistry::InitUOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab();
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsBySelectedTabID(
	const FName& InSelectedTabID) const
{
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate([InSelectedTabID](UListDataObject_Collection* Collection)

	{
		return Collection->GetDataID() == InSelectedTabID;
	});
	UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;
	TArray<UListDataObject_Base*> AllResultItems;

	for (auto ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData) continue;

		AllResultItems.Add(ChildListData);

		if (ChildListData->HasAnyChildListData())
		{
			FindChildListData(Cast<UListDataObject_Collection>(ChildListData), AllResultItems);
		}
	}

	return AllResultItems;
}

void UOptionsDataRegistry::FindChildListData(UListDataObject_Base* InCollection,
	TArray<UListDataObject_Base*>& OutResultItems) const
{
	if (!InCollection||!InCollection->HasAnyChildListData()) return;
	for (auto ChildListData : InCollection->GetAllChildListData())
	{
		if (!ChildListData) continue;
		OutResultItems.Add(ChildListData);
		if (ChildListData->HasAnyChildListData())
		{
			FindChildListData(ChildListData, OutResultItems);
		}
	}
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayCollectionData = NewObject<UListDataObject_Collection>();
	GameplayCollectionData->SetDataID(FName(TEXT("GameplayTabCollection")));
	GameplayCollectionData->SetDisplayName(FText::FromString(TEXT("Gameplay")));

	// TSharedPtr<FOptionsDataInteractionHelper*> ConstructHelper = MakeShared<FOptionsDataInteractionHelper*>(new FOptionsDataInteractionHelper(TEXT("GetCurrentGameDifficulty")));
	// GET_FUNCTION_NAME_CHECKED(UFrontendGameUserSettings, GetCurrentGameDifficulty);

	{
		UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
		GameDifficulty->SetDataID(FName(TEXT("GameDifficulty")));
		GameDifficulty->SetDisplayName(FText::FromString(TEXT("Difficulty")));
		GameDifficulty->SetDescriptionRichText(FText::FromString(TEXT("Adjust The Difficulty Of The Game Experience."
																"\n\n<Bold>Easy:</> For Casual Players, Less Challenging.\n\n"
																"<Bold>Normal:</> Standard Difficulty, Balanced Challenge.\n\n"
																"<Bold>Hard:</> For Experienced Players, Increased Challenge.\n\n"
	                                                            "<Bold>VeryHard:</> Extreme Difficulty, For Hardcore Players.")));
		GameDifficulty->AddDynamicOption(TEXT("Easy"), FText::FromString(TEXT("Easy")));
		GameDifficulty->AddDynamicOption(TEXT("Normal"), FText::FromString(TEXT("Normal")));
		GameDifficulty->AddDynamicOption(TEXT("Hard"), FText::FromString(TEXT("Hard")));
		GameDifficulty->AddDynamicOption(TEXT("VeryHard"), FText::FromString(TEXT("VeryHard")));
		GameDifficulty->SetDefaultValueString(TEXT("Normal"));
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		
		GameplayCollectionData->AddChildListData(GameDifficulty);
	}

	{
		UListDataObject_String* TestItem = NewObject<UListDataObject_String>();
		TestItem->SetDataID(FName(TEXT("TestItem")));
		TestItem->SetDisplayName(FText::FromString(TEXT("TestItem")));
		TestItem->SetDescriptionRichText(FText::FromString(TEXT("Test Image Item")));
		TestItem->SetSoftDescriptionImage(UWarriorFunctionLibrary::GetOptionsSoftImageByTag(WarriorGameplayTags::UI_Image_TestImage));
		GameplayCollectionData->AddChildListData(TestItem);
	}
	RegisteredOptionsTabCollections.Add(GameplayCollectionData);
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioCollectionData = NewObject<UListDataObject_Collection>();
	AudioCollectionData->SetDataID(FName(TEXT("AudioTabCollection")));
	AudioCollectionData->SetDisplayName(FText::FromString(TEXT("Audio")));
	//Volume
	{
		UListDataObject_Collection* VolumeCategoryCollection = NewObject<UListDataObject_Collection>();
		VolumeCategoryCollection->SetDataID(FName(TEXT("VolumeCategoryCollection")));
		VolumeCategoryCollection->SetDisplayName(FText::FromString(TEXT("Volume")));

		AudioCollectionData->AddChildListData(VolumeCategoryCollection);
		{
			UListDataObject_Scalar* OverAllVolume = NewObject<UListDataObject_Scalar>();
			OverAllVolume->SetDataID(FName(TEXT("OverAllVolume")));
			OverAllVolume->SetDisplayName(FText::FromString(TEXT("Overall Volume")));
			OverAllVolume->SetDescriptionRichText(FText::FromString(TEXT("Adjust The Overall Volume Of The Game.")));
			OverAllVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			OverAllVolume->SetOutPutValueRange(TRange<float>(0.f, 2.f));
			OverAllVolume->SetSliderStepSize(0.01f);
			OverAllVolume->SetDefaultValueString(LexToString(1.f));
			OverAllVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			OverAllVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			OverAllVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			OverAllVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));

			VolumeCategoryCollection->AddChildListData(OverAllVolume);
		}
		{
			UListDataObject_Scalar* MusicVolume = NewObject<UListDataObject_Scalar>();
			MusicVolume->SetDataID(FName(TEXT("Music Volume")));
			MusicVolume->SetDisplayName(FText::FromString(TEXT("Music Volume")));
			MusicVolume->SetDescriptionRichText(FText::FromString(TEXT("Adjust The Music Volume Of The Game.")));
			MusicVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			MusicVolume->SetOutPutValueRange(TRange<float>(0.f, 2.f));
			MusicVolume->SetSliderStepSize(0.01f);
			MusicVolume->SetDefaultValueString(LexToString(1.f));
			MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			MusicVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));

			VolumeCategoryCollection->AddChildListData(MusicVolume);
		}
	}
    //Sound
	{
		UListDataObject_Collection* SoundCategoryCollection = NewObject<UListDataObject_Collection>();
		SoundCategoryCollection->SetDataID(FName(TEXT("SoundCategoryCollection")));
		SoundCategoryCollection->SetDisplayName(FText::FromString(TEXT("Sound")));

		AudioCollectionData->AddChildListData(SoundCategoryCollection);
		{
			UListDataObject_StringBool* AllowBackGroundAudio = NewObject<UListDataObject_StringBool>();
			AllowBackGroundAudio->SetDataID(FName(TEXT("AllowBackGroundAudio")));
			AllowBackGroundAudio->SetDisplayName(FText::FromString(TEXT("Allow Background Audio")));
			AllowBackGroundAudio->SetDescriptionRichText(FText::FromString(TEXT("Allow The Game To Play Audio In The Background.")));
			AllowBackGroundAudio->SetFalseAsDefaultValue();
			AllowBackGroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackGroundAudioVolume));
			AllowBackGroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackGroundAudioVolume));
			AllowBackGroundAudio->SetShouldApplySettingsImmediately(true);

			SoundCategoryCollection->AddChildListData(AllowBackGroundAudio);
		}
	}
	RegisteredOptionsTabCollections.Add(AudioCollectionData);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoCollectionData = NewObject<UListDataObject_Collection>();
	VideoCollectionData->SetDataID(FName(TEXT("VideoTabCollection")));
	VideoCollectionData->SetDisplayName(FText::FromString(TEXT("Video")));

	RegisteredOptionsTabCollections.Add(VideoCollectionData);
}

void UOptionsDataRegistry::InitControlCollectionTab()
{
	UListDataObject_Collection* ControlCollectionData = NewObject<UListDataObject_Collection>();
	ControlCollectionData->SetDataID(FName(TEXT("ControlTabCollection")));
	ControlCollectionData->SetDisplayName(FText::FromString(TEXT("Control")));

	RegisteredOptionsTabCollections.Add(ControlCollectionData);
}
