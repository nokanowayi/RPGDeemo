// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WarriorSessionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UWarriorSessionSubsystem();
	
	UFUNCTION(blueprintCallable)
	void CreateSession(int32 Num,FString SessionName);

	void FindSession(int32 MaxSearchResult);

	void JoinSession(const FOnlineSessionSearchResult& SearchResult);

	void DestroySession();

	void StartSession();

protected:
	
	void OnCreateGameSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindGameSessionComplete(bool bWasSuccessful);
	void OnJoinGameSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroyGameSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartGameSessionComplete(FName SessionName, bool bWasSuccessful);

protected:
	IOnlineSessionPtr OnlineSessionInterface;
	
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
	

};
