// MyRPGDemo


#include "Widget/Options/OptionsDataRegistry.h"

#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Widget/Options/DataObject/ListDataObject_String.h"
#include "Widget/Options/DataObject/ListDataObject_Collection.h"

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
	return FoundTabCollection->GetAllChildListData();
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
