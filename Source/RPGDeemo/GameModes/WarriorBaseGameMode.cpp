// MyRPGDemo


#include "WarriorBaseGameMode.h"
#include "Controllers/WarriorHeroController.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

AWarriorBaseGameMode::AWarriorBaseGameMode()
{
	PlayerControllerClass = AWarriorHeroController::StaticClass();

	// 多人：每个玩家默认生成间隔（避免重叠）
	bStartPlayersAsSpectators = false;
}

void AWarriorBaseGameMode::HostLANMatch(int32 MaxPlayers)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("HostLANMatch: 无 OnlineSubsystem"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("HostLANMatch: 无 SessionInterface"));
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;          // 局域网匹配
	SessionSettings.bShouldAdvertise = true;     // 广播可被发现
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bUsesPresence = false;

	SessionInterface->CreateSession(0, FName("WarriorGameSession"), SessionSettings);
	UE_LOG(LogTemp, Log, TEXT("HostLANMatch: 创建房间 最大%d人"), MaxPlayers);
}

void AWarriorBaseGameMode::JoinLANMatch()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinLANMatch: 无 OnlineSubsystem"));
		return;
	}

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinLANMatch: 无 SessionInterface"));
		return;
	}

	// 查找局域网会话
	FOnlineSessionSearch SearchSettings;
	SearchSettings.bIsLanQuery = true;
	SearchSettings.MaxSearchResults = 10;

	SessionInterface->FindSessions(0, MakeShareable(new FOnlineSessionSearch(SearchSettings)));
	UE_LOG(LogTemp, Log, TEXT("JoinLANMatch: 开始查找局域网房间"));
}
