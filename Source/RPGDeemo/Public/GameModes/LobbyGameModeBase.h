// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
};
