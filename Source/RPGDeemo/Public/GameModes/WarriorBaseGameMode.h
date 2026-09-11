// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorBaseGameMode.generated.h"

/**
 * 多人共斗基础 GameMode
 * 支持局域网会话创建/加入（后续可换 EOS/Steam）
 */
UCLASS()
class RPGDEEMO_API AWarriorBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWarriorBaseGameMode();

	/** 创建局域网会话（Host） */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Multiplayer")
	void HostLANMatch(int32 MaxPlayers = 4);

	/** 查找并加入局域网会话（Client） */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Multiplayer")
	void JoinLANMatch();

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "GameSettings")
	EWarriorGameDifficulty CurrentGameDifficulty;

public:
	FORCEINLINE EWarriorGameDifficulty GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
};
