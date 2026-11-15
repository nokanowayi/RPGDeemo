// MyRPGDemo


#include "Subsystems/WarriorSessionSubsystem.h"

#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "WarriorDebugHelper.h"

UWarriorSessionSubsystem::UWarriorSessionSubsystem():
    OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnCreateGameSessionComplete)),
    OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnFindGameSessionComplete)),
    OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnJoinGameSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnDestroyGameSessionComplete)),
	OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnStartGameSessionComplete))
{
}

bool UWarriorSessionSubsystem::ResolveSessionInterface()
{
	// Keep pending delegates attached to the interface which owns their requests.
	if (!OnCreateSessionCompleteDelegateHandle.IsValid()
		&& !OnFindSessionsCompleteDelegateHandle.IsValid()
		&& !OnJoinSessionCompleteDelegateHandle.IsValid()
		&& !OnDestroySessionCompleteDelegateHandle.IsValid())
	{
		OnlineSessionInterface = GetWorld() ? Online::GetSessionInterface(GetWorld()) : nullptr;
	}
	return OnlineSessionInterface.IsValid();
}

void UWarriorSessionSubsystem::Deinitialize()
{
	if (OnlineSessionInterface.IsValid())
	{
		OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		OnlineSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
	}
	OnCreateSessionCompleteDelegateHandle.Reset();
	OnFindSessionsCompleteDelegateHandle.Reset();
	OnJoinSessionCompleteDelegateHandle.Reset();
	OnDestroySessionCompleteDelegateHandle.Reset();
	OnStartSessionCompleteDelegateHandle.Reset();
	OnlineSessionInterface.Reset();
	Super::Deinitialize();
}

void UWarriorSessionSubsystem::CreateSession(int32 Num,FString SessionName)
{
	if (OnCreateSessionCompleteDelegateHandle.IsValid() || OnDestroySessionCompleteDelegateHandle.IsValid())
	{
		OnMultiplayCreateSessionCompleteDelegate.Broadcast(false);
		return;
	}
	if (!ResolveSessionInterface())
	{
		UE_LOG(LogTemp, Error, TEXT("CreateSession: OnlineSessionInterface is unavailable."));
		return;
	}

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);

	if (ExistingSession!=nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = Num;
		LastMatchType = SessionName;
		
		DestroySession();
		return;
	}

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	LastSessionSettings = MakeShared<FOnlineSessionSettings>();
	LastSessionSettings->bIsLANMatch = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL";
	LastSessionSettings->NumPublicConnections = Num;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUseLobbiesIfAvailable = !LastSessionSettings->bIsLANMatch;
	LastSessionSettings->bUsesPresence = LastSessionSettings->bUseLobbiesIfAvailable;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = LastSessionSettings->bUsesPresence;
	LastSessionSettings->Set(FName("MatchType"), SessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	OnCreateSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid()
		|| !OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		OnCreateSessionCompleteDelegateHandle.Reset();
		OnMultiplayCreateSessionCompleteDelegate.Broadcast(false);
		UE_LOG(LogTemp, Error, TEXT("CreateSession: request rejected before completion."));
	}
}

void UWarriorSessionSubsystem::FindSession(int32 MaxSearchResult)
{
	if (!ResolveSessionInterface())
	{
		UE_LOG(LogTemp, Error, TEXT("FindSession: OnlineSessionInterface is unavailable."));
		return;
	}
	if (MaxSearchResult <= 0 || OnFindSessionsCompleteDelegateHandle.IsValid() || OnDestroySessionCompleteDelegateHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSession: invalid max search result or request already pending."));
		return;
	}

	LastSessionSearch = MakeShared<FOnlineSessionSearch>();
	LastSessionSearch->MaxSearchResults = MaxSearchResult;
	LastSessionSearch->bIsLanQuery = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL";
	if (!LastSessionSearch->bIsLanQuery)
	{
		LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	OnFindSessionsCompleteDelegateHandle = OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();

	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid() || !OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		OnFindSessionsCompleteDelegateHandle.Reset();
		OnMultiplayFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(),false);
		UE_LOG(LogTemp, Error, TEXT("FindSession: request rejected before completion."));
	}
}

void UWarriorSessionSubsystem::JoinSession_ByBluePrint(int32 Count)
{
}

void UWarriorSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!ResolveSessionInterface() || OnJoinSessionCompleteDelegateHandle.IsValid() || OnDestroySessionCompleteDelegateHandle.IsValid())
	{
		OnMultiplayJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		UE_LOG(LogTemp, Error, TEXT("JoinSession: OnlineSessionInterface is unavailable."));
		return;
	}
	OnJoinSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid()
		|| !OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult))
	{
		OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		OnJoinSessionCompleteDelegateHandle.Reset();
		OnMultiplayJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		UE_LOG(LogTemp, Error, TEXT("JoinSession: request rejected before completion."));
	}
}

void UWarriorSessionSubsystem::DestroySession()
{
	if (OnDestroySessionCompleteDelegateHandle.IsValid())
	{
		return;
	}
	if (!ResolveSessionInterface())
	{
		UE_LOG(LogTemp, Error, TEXT("DestroySession: OnlineSessionInterface is unavailable."));
		OnDestroyGameSessionComplete(NAME_GameSession, false);
		return;
	}
	if (!OnlineSessionInterface->GetNamedSession(NAME_GameSession))
	{
		OnDestroyGameSessionComplete(NAME_GameSession, true);
		return;
	}

	OnDestroySessionCompleteDelegateHandle = OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
	
	if (!OnlineSessionInterface->DestroySession(NAME_GameSession))
	{
		OnDestroyGameSessionComplete(NAME_GameSession, false);
	}
}

void UWarriorSessionSubsystem::LeaveSession()
{
	// Explicitly leaving must never activate the recreate-session branch.
	bCreateSessionOnDestroy = false;
	LastNumPublicConnections = 0;
	LastMatchType.Reset();
	DestroySession();
}

void UWarriorSessionSubsystem::StartSession()
{
}

void UWarriorSessionSubsystem::OnCreateGameSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionName != NAME_GameSession)
	{
		return;
	}
	if (OnlineSessionInterface.IsValid())
	{
		OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}
	OnCreateSessionCompleteDelegateHandle.Reset();
	if (bWasSuccessful)
	{
		OnMultiplayCreateSessionCompleteDelegate.Broadcast(true);
		Debug::Print(TEXT("Create Game Session Success!"));
		UWorld* World = GetWorld();
		if (!World || World->GetNetMode() == NM_Client)
		{
			UE_LOG(LogTemp, Error, TEXT("CreateSession: Lobby travel requires a host world, not a client."));
			return;
		}
		if (!World->ServerTravel(TEXT("/Game/Maps/Lobby?listen")))
		{
			UE_LOG(LogTemp, Error, TEXT("CreateSession: ServerTravel to Lobby was rejected."));
		}
	}
	else
	{
		Debug::Print(TEXT("Create Game Session Failed!"));
	}
}

void UWarriorSessionSubsystem::OnFindGameSessionComplete(bool bWasSuccessful)
{
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
	OnFindSessionsCompleteDelegateHandle.Reset();
	if (!LastSessionSearch.IsValid())
	{
		OnMultiplayFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	if (LastSessionSearch->SearchResults.Num() <=0)
	{
		OnMultiplayFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(),bWasSuccessful);
		return;
	}
	OnMultiplayFindSessionsCompleteDelegate.Broadcast(LastSessionSearch->SearchResults,bWasSuccessful);

	for (FOnlineSessionSearchResult& SearchResult : LastSessionSearch->SearchResults)
	{
		FString SessionTypeSetting;
		SearchResult.Session.SessionSettings.Get(FName("MatchType"), SessionTypeSetting);
		if (SessionTypeSetting == MatchType)
		{
			Debug::Print(FString::Printf(TEXT("Find Game Session Success! Session Type: %s"), *SessionTypeSetting));
			JoinSession(SearchResult);
			return;
		}
		else
		{
			Debug::Print(TEXT("Find Game Session Success! Session Type: Unknown"));
		}
	}
}

void UWarriorSessionSubsystem::OnJoinGameSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionName != NAME_GameSession)
	{
		return;
	}
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}
	OnJoinSessionCompleteDelegateHandle.Reset();

	OnMultiplayJoinSessionCompleteDelegate.Broadcast(Result);

	if (Result == EOnJoinSessionCompleteResult::Success && OnlineSessionInterface.IsValid())
	{
		FString Address;
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController && OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address) && !Address.IsEmpty())
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
	
}

void UWarriorSessionSubsystem::OnDestroyGameSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionName != NAME_GameSession)
	{
		return;
	}
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}
	OnDestroySessionCompleteDelegateHandle.Reset();
	const bool bRecreateSession = bWasSuccessful && bCreateSessionOnDestroy;
	bCreateSessionOnDestroy = false;
	if (bRecreateSession)
	{
		CreateSession(LastNumPublicConnections, LastMatchType);
	}
	OnMultiplayDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);
}

void UWarriorSessionSubsystem::OnStartGameSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

