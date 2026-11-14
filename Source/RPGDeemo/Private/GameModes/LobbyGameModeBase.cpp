// MyRPGDemo


#include "GameModes/LobbyGameModeBase.h"

#include "WarriorDebugHelper.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GameState)
	{
		int32 PlayerNums = GameState.Get()->PlayerArray.Num();

		Debug::Print(FString::Printf(TEXT("Player %s has joined the lobby. Total players: %d"), *NewPlayer->GetName(), PlayerNums), 5.f, FColor::Green);
	}

	APlayerState* NewPlayerState = NewPlayer->GetPlayerState<APlayerState>();
	if (NewPlayerState)
	{
		Debug::Print(FString::Printf(TEXT("Player %s has joined the lobby. Total) players: %d"), *NewPlayerState->GetPlayerName(), GameState.Get()->PlayerArray.Num()), 5.f, FColor::Green);
	}
}

void ALobbyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
