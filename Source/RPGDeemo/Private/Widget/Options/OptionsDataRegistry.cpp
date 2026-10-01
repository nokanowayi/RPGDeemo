// MyRPGDemo


#include "Widget/Options/OptionsDataRegistry.h"

#include "Widget/Options/DataObject/ListDataObject_Collection.h"

void UOptionsDataRegistry::InitUOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab();
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayCollectionData = NewObject<UListDataObject_Collection>();
	GameplayCollectionData->SetDataID(FName(TEXT("GameplayTabCollection")));
	GameplayCollectionData->SetDisplayName(FText::FromString(TEXT("Gameplay")));

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
