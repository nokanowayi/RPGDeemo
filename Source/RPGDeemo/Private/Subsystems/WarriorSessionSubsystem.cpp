// MyRPGDemo


#include "Subsystems/WarriorSessionSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "WarriorDebugHelper.h"

UWarriorSessionSubsystem::UWarriorSessionSubsystem():
    OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnCreateGameSessionComplete)),
    OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnJoinGameSessionComplete)),
    OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnFindGameSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnDestroyGameSessionComplete)),
	OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &UWarriorSessionSubsystem::OnStartGameSessionComplete))
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UWarriorSessionSubsystem::CreateSession(int32 Num,FString SessionName)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);

	if (ExistingSession!=nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}
	FOnlineSessionSettings SessionSettings;

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = Num;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate); 
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
}

void UWarriorSessionSubsystem::FindSession(int32 MaxSearchResult)
{
}

void UWarriorSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
}

void UWarriorSessionSubsystem::DestroySession()
{
}

void UWarriorSessionSubsystem::StartSession()
{
}

void UWarriorSessionSubsystem::OnCreateGameSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		Debug::Print(TEXT("Create Game Session Success!"));
	}
	else
	{
		Debug::Print(TEXT("Create Game Session Failed!"));
	}
}

void UWarriorSessionSubsystem::OnFindGameSessionComplete(bool bWasSuccessful)
{
}

void UWarriorSessionSubsystem::OnJoinGameSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UWarriorSessionSubsystem::OnDestroyGameSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UWarriorSessionSubsystem::OnStartGameSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

