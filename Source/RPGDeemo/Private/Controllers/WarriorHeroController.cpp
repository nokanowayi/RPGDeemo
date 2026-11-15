// MyRPGDemo


#include "Controllers/WarriorHeroController.h"

#include "Engine/GameInstance.h"
#include "Engine/NetConnection.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/WarriorSessionSubsystem.h"

namespace WarriorMatchMaps
{
	const FName MainMenu(TEXT("/Game/Maps/MainMenuMap"));
	const FName Gameplay(TEXT("/Game/Maps/GameModeTestMap"));
}

AWarriorHeroController::AWarriorHeroController()
{
	TeamID = FGenericTeamId(0);
}

FGenericTeamId AWarriorHeroController::GetGenericTeamId() const
{
	return TeamID;
}

void AWarriorHeroController::ReturnToMainMenuFromMatch()
{
	if (!IsLocalController() || bMatchActionPending || !GetWorld())
	{
		return;
	}
	bMatchActionPending = true;

	if (HasAuthority())
	{
		NotifyClientsHostIsLeaving();
	}

	LeaveLocalSession();
	UGameplayStatics::OpenLevel(this, WarriorMatchMaps::MainMenu, true);
}

void AWarriorHeroController::RestartMatch()
{
	UWorld* World = GetWorld();
	if (!IsLocalController() || bMatchActionPending || !World)
	{
		return;
	}
	bMatchActionPending = true;

	if (HasAuthority())
	{
		if (World->GetNetMode() == NM_Standalone)
		{
			UGameplayStatics::OpenLevel(this, WarriorMatchMaps::Gameplay, true);
		}
		// 保留房间当前监听端口（包括 PIE 或命令行指定的端口），供客户端重新连接。
		else if (!World->ServerTravel(WarriorMatchMaps::Gameplay.ToString() + TEXT("?listen"), false))
		{
			bMatchActionPending = false;
			UE_LOG(LogTemp, Error, TEXT("RestartMatch: ServerTravel to GameModeTestMap was rejected."));
		}
		return;
	}

	// 普通本地函数，不向服务器发起重开 RPC；客户端离房后单机重开。
	LeaveLocalSession();
	UGameplayStatics::OpenLevel(this, WarriorMatchMaps::Gameplay, true);
}

void AWarriorHeroController::QuitMatch()
{
	if (!IsLocalController() || bMatchActionPending || !GetWorld())
	{
		return;
	}
	bMatchActionPending = true;

	if (HasAuthority())
	{
		NotifyClientsHostIsLeaving();
	}

	LeaveLocalSession();
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void AWarriorHeroController::NotifyClientsHostIsLeaving()
{
	if (!HasAuthority())
	{
		return;
	}

	const FText Reason = NSLOCTEXT("Warrior", "HostLeftMatch", "The host has left the game.");
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* Controller = It->Get();
		if (Controller && !Controller->IsLocalController())
		{
			Controller->ClientReturnToMainMenuWithTextReason(Reason);
			if (UNetConnection* Connection = Controller->GetNetConnection())
			{
				// 退出进程前发送已排队的可靠通知，避免等到下一次网络 Tick。
				Connection->FlushNet();
			}
		}
	}
}

void AWarriorHeroController::ClientReturnToMainMenuWithTextReason_Implementation(const FText& ReturnReason)
{
	LeaveLocalSession();
	// 引擎解除连接并加载 GameDefaultMap（本项目配置为 MainMenuMap）。
	Super::ClientReturnToMainMenuWithTextReason_Implementation(ReturnReason);
}

void AWarriorHeroController::LeaveLocalSession()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UWarriorSessionSubsystem* Sessions = GameInstance->GetSubsystem<UWarriorSessionSubsystem>())
		{
			Sessions->LeaveSession();
		}
	}
}
