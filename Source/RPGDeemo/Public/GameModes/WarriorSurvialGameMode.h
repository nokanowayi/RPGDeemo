// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "Engine/DataTable.h"
#include "GameStates/WarriorSurvialGameState.h"   // 为了 FOnSurvivalGameStateChange（GameState 转发用）
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorSurvialGameMode.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo 
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	UPROPERTY(EditAnywhere)
	int32 MinPerSpawnCount = 1;
	UPROPERTY(EditAnywhere)
	int32 MaxPerSpawnCount = 3;
	
	
};

USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow:public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerInfos;

	UPROPERTY(EditAnywhere)
	int32 TotalEnemyToSpawnThisWave = 1;
};

UCLASS()
class RPGDEEMO_API AWarriorSurvialGameMode : public AWarriorBaseGameMode
{
	GENERATED_BODY()

public:
	AWarriorSurvialGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	

private:
	/** 当前的生存模式 GameState（GameStateClass 已在构造函数里指定），可能为空，调用处判空 */
	class AWarriorSurvialGameState* GetSurvialGameState() const;

	void SetCurrentState(ESurvivalGameModeState NewState);
	bool HasFinishedAllWaves() const;
	void PreLoadNextWaveEnemies();
	int32 TrySpawnWaveEnemies();
	FWarriorEnemyWaveSpawnerTableRow GetCurrentWaveSpawnerTableRow() const;
	bool ShouldSpawnEnemies() const;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION()
	void OnEnemyDestroy(AActor* DestroyedActor);

	// 注意：状态字段与"给客户端看"的状态变化委托都在 AWarriorSurvialGameState 上。
	// GameMode 只存在于服务器，放在这里的状态客户端拿不到（生存模式 UI 曾经因此只在服务器可见）。
	UPROPERTY()
	ESurvivalGameModeState CurrentGameModeState;

	/**
	 * 服务器内部的旧委托，仅为兼容已有蓝图而保留。
	 * 客户端侧请改绑 AWarriorSurvialGameState::OnCurrentGameStateChange（GameMode 在客户端不存在）。
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSurvivalGameStateChange OnCurrentGameStateChange;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess="true"))
	UDataTable* EnemyWaveSpawnerDataTable;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess="true"))
	int32 TotalWaveToSpawn;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess="true"))
	int32 CurrentWaveCount = 1;   // 行名规则 Wave1、Wave2…：从 1 开始，与 DT_EnemyWaveSpawner 的行名对应

	UPROPERTY()
	int32 CurrentSpawnedEnemiesCounter = 0;

	UPROPERTY()
	int32 TotalSpawnThisWaveCounter = 0;

	UPROPERTY()
	TArray<AActor*> TargetPointsArray;
	
	UPROPERTY()
	float TimePassSinceStart = 0.f;

	UPROPERTY()
	TMap<TSoftClassPtr<AWarriorEnemyCharacter>,UClass*> PreloadedEnemyClassMap;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess="true"))
	float SpawnWaitTime = 5.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="WaveDefinition",meta=(AllowPrivateAccess="true"))
	float SpawnEnemyDelayTime = 2.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess="true"))
    float WaveCompletedWaitTime = 5.f;
public:
	UFUNCTION(BlueprintCallable)
	void RegisterSpawnedEnemy(const TArray<AWarriorEnemyCharacter*>& InEnemies);
};
