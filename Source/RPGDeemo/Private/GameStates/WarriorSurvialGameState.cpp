// MyRPGDemo

#include "GameStates/WarriorSurvialGameState.h"

#include "Net/UnrealNetwork.h"

void AWarriorSurvialGameState::SetCurrentGameModeState(ESurvivalGameModeState NewState)
{
	// 这里故意不做"值相同就跳过"的判重：
	//   · 服务器的 GameMode 在 BeginPlay 时会用初始状态调用一次，主机自己的 UI 要靠这次广播出现；
	//   · 客户端靠 OnRep_CurrentGameModeState（含中途加入时的初始复制）广播，与这里无关。
	// SetCurrentState 只在状态真正切换时被调用，所以不会产生冗余广播。
	CurrentGameModeState = NewState;

	OnCurrentGameStateChange.Broadcast(CurrentGameModeState);
}

void AWarriorSurvialGameState::SetWaveInfo(int32 InCurrentWaveCount, int32 InTotalWaveToSpawn)
{
	CurrentWaveCount = InCurrentWaveCount;
	TotalWaveToSpawn = InTotalWaveToSpawn;
}

void AWarriorSurvialGameState::SetWaveTimings(float InSpawnWaitTime, float InSpawnEnemyDelayTime, float InWaveCompletedWaitTime)
{
	SpawnWaitTime = InSpawnWaitTime;
	SpawnEnemyDelayTime = InSpawnEnemyDelayTime;
	WaveCompletedWaitTime = InWaveCompletedWaitTime;
}

void AWarriorSurvialGameState::OnRep_CurrentGameModeState()
{
	// 客户端（以及中途加入的玩家：初始复制也会走一次 OnRep）
	OnCurrentGameStateChange.Broadcast(CurrentGameModeState);
}

void AWarriorSurvialGameState::OnRep_WaveInfo()
{
	// 波次信息变化时也广播，让客户端 UI 刷新"第N波 / 共N波"
	OnCurrentGameStateChange.Broadcast(CurrentGameModeState);
}

void AWarriorSurvialGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWarriorSurvialGameState, CurrentGameModeState);
	DOREPLIFETIME(AWarriorSurvialGameState, CurrentWaveCount);
	DOREPLIFETIME(AWarriorSurvialGameState, TotalWaveToSpawn);
	DOREPLIFETIME(AWarriorSurvialGameState, SpawnWaitTime);
	DOREPLIFETIME(AWarriorSurvialGameState, SpawnEnemyDelayTime);
	DOREPLIFETIME(AWarriorSurvialGameState, WaveCompletedWaitTime);
}
