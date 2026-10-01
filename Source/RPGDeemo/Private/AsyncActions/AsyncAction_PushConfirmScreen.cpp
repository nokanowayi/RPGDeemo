// MyRPGDemo


#include "AsyncActions/AsyncAction_PushConfirmScreen.h"

#include "Subsystems/FrontendUISubsystem.h"

UAsyncAction_PushConfirmScreen* UAsyncAction_PushConfirmScreen::PushConfirmScreenToModalStackAsync(
	UObject* WorldContextObject, EConfirmScreenType InConfirmScreenType, const FText InScreenTitle,
	const FText InScreenMessage)
{if (GEngine)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		UAsyncAction_PushConfirmScreen* Node = NewObject<UAsyncAction_PushConfirmScreen>();
		Node->CachedOwingWorld = World;
		Node->CachedConfirmScreenType = InConfirmScreenType;
		Node->CachedScreenTitle = InScreenTitle;
		Node->CachedScreenMessage = InScreenMessage;
			
		Node->RegisterWithGameInstance(World);

		return Node;
	}
}

	return nullptr;
}

void UAsyncAction_PushConfirmScreen::Activate()
{
	UFrontendUISubsystem::Get(CachedOwingWorld.Get())->PushConfirmScreenToModalStackAsync(CachedConfirmScreenType,CachedScreenTitle,CachedScreenMessage,
		[this](EConfirmScreenButtonType InClickedButtonType)
		{
			OnButtonClicked.Broadcast(InClickedButtonType);
			SetReadyToDestroy();
		});
}
