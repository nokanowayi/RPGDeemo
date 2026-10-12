// MyRPGDemo


#include "GameInstance/WarriorGameInstance.h"

#include "MoviePlayer.h"

void UWarriorGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UWarriorGameInstance::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UWarriorGameInstance::OnDestinationWorldLoaded);
}

void UWarriorGameInstance::Shutdown()
{
	// FCoreUObjectDelegates 是全局的：Init 里 AddUObject 之后如果不解绑，
	// 全局委托会一直强引用本 GameInstance，连带它的 World / 控件 / 子系统都无法被回收。
	// 表现就是退出 PIE 时刷：
	//   World /Game/Maps/UEDPIE_0_xxx 仍然在被引用
	//   BP_WarriorGameInstance_C /Engine/Transient.UnrealEdEngine_0:... 仍然在被引用
	// 而且每跑一次 PIE 就多漏一份。
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	Super::Shutdown();
}

void UWarriorGameInstance::OnPreLoadMap(const FString& MapName)
{
	// 专用服务器没有本地玩家、也没有 MoviePlayer，直接跳过
	if (IsRunningDedicatedServer() || !GetMoviePlayer())
	{
		return;
	}

	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.0f;
	LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void UWarriorGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	if (IsRunningDedicatedServer() || !GetMoviePlayer())
	{
		return;
	}

	GetMoviePlayer()->StopMovie();
}

TSoftObjectPtr<UWorld> UWarriorGameInstance::GetLevelByTag(FGameplayTag LevelTag)
{
	for (const FWarriorGameLevelSet& GameLevelSet : GameLevelSets)
	{
		if (!GameLevelSet.IsValid()) continue;

		if (GameLevelSet.LevelTag == LevelTag)
		{
			return GameLevelSet.Level;
		}
	}

	return nullptr;
}
