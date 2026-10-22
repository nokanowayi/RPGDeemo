// MyRPGDemo


#include "Widget/Options/OptionsDataRegistry.h"
#include "Widget/Options/DataObject/ListDataObject_String.h"
#include "Widget/Options/DataObject/ListDataObject_Collection.h"

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

	{
		UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
		GameDifficulty->SetDataID(FName(TEXT("GameDifficulty")));
		GameDifficulty->SetDisplayName(FText::FromString(TEXT("Difficulty")));
		GameDifficulty->AddDynamicOption(TEXT("Easy"), FText::FromString(TEXT("Easy")));
		GameDifficulty->AddDynamicOption(TEXT("Normal"), FText::FromString(TEXT("Normal")));
		GameDifficulty->AddDynamicOption(TEXT("Hard"), FText::FromString(TEXT("Hard")));
		GameDifficulty->AddDynamicOption(TEXT("VeryHard"), FText::FromString(TEXT("VeryHard")));
		GameplayCollectionData->AddChildListData(GameDifficulty);
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
