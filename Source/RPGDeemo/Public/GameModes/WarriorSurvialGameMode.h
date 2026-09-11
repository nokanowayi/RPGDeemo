// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "Engine/DataTable.h"
#include "WarriorSurvialGameMode.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESurvivalGameModeState : uint8
{
	WaitSpawnNewWave,
	SpawningNewWave,
	InProgress,
	WaveComplete,
	AllWaveDone,
	PlayerDie
};

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameStateChange, ESurvivalGameModeState, CurrentState);
UCLASS()
class RPGDEEMO_API AWarriorSurvialGameMode : public AWarriorBaseGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	

private:
	void SetCurrentState(ESurvivalGameModeState NewState);
	bool HasFinishedAllWaves() const;
	void PreLoadNextWaveEnemies();
	int32 TrySpawnWaveEnemies();
	FWarriorEnemyWaveSpawnerTableRow GetCurrentWaveSpawnerTableRow() const;
	bool ShouldSpawnEnemies() const;

	UFUNCTION()
	void OnEnemyDestroy(AActor* DestroyedActor);
	UPROPERTY()
	ESurvivalGameModeState CurrentGameModeState;

	UPROPERTY(BlueprintAssignable,BlueprintCallable)
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
