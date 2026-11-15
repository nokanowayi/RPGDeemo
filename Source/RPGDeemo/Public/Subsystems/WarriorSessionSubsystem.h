// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WarriorSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayOnCreateSessionCompleteDelegate,bool,bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayOnFindSessionsCompleteDelegate,const TArray<FOnlineSessionSearchResult>& Results,bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayOnJoinSessionCompleteDelegate,EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayOnDestroySessionCompleteDelegate,bool,bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayOnStartSessionCompleteDelegate,bool,bWasSuccessful);
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UWarriorSessionSubsystem();
	virtual void Deinitialize() override;
	
	UFUNCTION(blueprintCallable)
	void CreateSession(int32 Num,FString SessionName);

	UFUNCTION(blueprintCallable)
	void FindSession(int32 MaxSearchResult);

	UFUNCTION(blueprintCallable)
	void JoinSession_ByBluePrint(int32 Count);
	
	void JoinSession(const FOnlineSessionSearchResult& SearchResult);

	void DestroySession();
	void LeaveSession();

	void StartSession();

	int32 NumConnected{4};
	FString MatchType{TEXT("FreeForAll")};
protected:
	
	void OnCreateGameSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindGameSessionComplete(bool bWasSuccessful);
	void OnJoinGameSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroyGameSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartGameSessionComplete(FName SessionName, bool bWasSuccessful);

	FMultiplayOnCreateSessionCompleteDelegate OnMultiplayCreateSessionCompleteDelegate;
	FMultiplayOnFindSessionsCompleteDelegate OnMultiplayFindSessionsCompleteDelegate;
	FMultiplayOnJoinSessionCompleteDelegate OnMultiplayJoinSessionCompleteDelegate;
	FMultiplayOnDestroySessionCompleteDelegate OnMultiplayDestroySessionCompleteDelegate;
	FMultiplayOnStartSessionCompleteDelegate OnMultiplayStartSessionCompleteDelegate;

protected:
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	bool bCreateSessionOnDestroy{false};
	int32 LastNumPublicConnections{4};
	FString LastMatchType;

private:
	bool ResolveSessionInterface();
};
