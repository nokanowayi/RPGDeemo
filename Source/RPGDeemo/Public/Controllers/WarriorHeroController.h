// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorHeroController.generated.h"

/**
 *
 */
UCLASS()
class RPGDEEMO_API AWarriorHeroController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AWarriorHeroController();
	virtual FGenericTeamId GetGenericTeamId() const override;

	/** 房主通知全员返回菜单；客户端只退出自己的房间。请在本地 Owning Player 上调用。 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Multiplayer|Match")
	void ReturnToMainMenuFromMatch();

	/** 房主带全员重开 GameModeTestMap；客户端离开房间后在本机重开。 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Multiplayer|Match")
	void RestartMatch();

	/** 房主先通知其他玩家返回菜单再退出；客户端只退出本机游戏。 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Multiplayer|Match")
	void QuitMatch();

protected:
	virtual void ClientReturnToMainMenuWithTextReason_Implementation(const FText& ReturnReason) override;

private:
	void NotifyClientsHostIsLeaving();
	void LeaveLocalSession();

	bool bMatchActionPending = false;
	FGenericTeamId TeamID;
};
