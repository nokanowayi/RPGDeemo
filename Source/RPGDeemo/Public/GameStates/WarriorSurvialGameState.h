// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorSurvialGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameStateChange, ESurvivalGameModeState, CurrentState);

UCLASS()
class RPGDEEMO_API AWarriorSurvialGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void SetCurrentGameModeState(ESurvivalGameModeState NewState);

	/** 服务器写入波次信息（当前第几波 / 共几波） */
	void SetWaveInfo(int32 InCurrentWaveCount, int32 InTotalWaveToSpawn);

	/** 服务器写入波次计时配置（UI 要靠它显示倒计时，客户端读不到 GameMode 上的配置） */
	void SetWaveTimings(float InSpawnWaitTime, float InSpawnEnemyDelayTime, float InWaveCompletedWaitTime);

	UFUNCTION(BlueprintPure, Category = "Warrior|Survival")
	ESurvivalGameModeState GetCurrentGameModeState() const { return CurrentGameModeState; }

	UFUNCTION(BlueprintPure, Category = "Warrior|Survival")
	int32 GetCurrentWaveCount() const { return CurrentWaveCount; }

	UFUNCTION(BlueprintPure, Category = "Warrior|Survival")
	int32 GetTotalWaveToSpawn() const { return TotalWaveToSpawn; }

	UFUNCTION(BlueprintPure, Category = "Warrior|Survival")
	float GetSpawnWaitTime() const { return SpawnWaitTime; }

	UFUNCTION(BlueprintPure, Category = "Warrior|Survival")
	float GetSpawnEnemyDelayTime() const { return SpawnEnemyDelayTime; }

	UFUNCTION(BlueprintPure, Category = "Warrior|Survival")
	float GetWaveCompletedWaitTime() const { return WaveCompletedWaitTime; }

	/** 状态变化委托：服务器写完本地触发，客户端在 OnRep 里触发 —— UI 绑这个 */
	UPROPERTY(BlueprintAssignable, Category = "Warrior|Survival")
	FOnSurvivalGameStateChange OnCurrentGameStateChange;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentGameModeState, BlueprintReadOnly, Category = "Warrior|Survival")
	ESurvivalGameModeState CurrentGameModeState = ESurvivalGameModeState::WaitSpawnNewWave;

	UPROPERTY(ReplicatedUsing = OnRep_WaveInfo, BlueprintReadOnly, Category = "Warrior|Survival")
	int32 CurrentWaveCount = 1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Warrior|Survival")
	int32 TotalWaveToSpawn = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Warrior|Survival")
	float SpawnWaitTime = 5.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Warrior|Survival")
	float SpawnEnemyDelayTime = 2.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Warrior|Survival")
	float WaveCompletedWaitTime = 5.f;

	UFUNCTION()
	void OnRep_CurrentGameModeState();

	/** 波次信息变化时也广播一次，否则客户端 UI 会停在旧的"第N波" */
	UFUNCTION()
	void OnRep_WaveInfo();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
