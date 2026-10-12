// MyRPGDemo


#include "GameModes/WarriorSurvialGameMode.h"

#include "NavigationSystem.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "GameStates/WarriorSurvialGameState.h"
#include "Kismet/GameplayStatics.h"

AWarriorSurvialGameMode::AWarriorSurvialGameMode()
{
	// 生存模式要展示给客户端的状态与委托都放在 GameState 上。
	// GameMode 只存在于服务器 —— 原来 UI 直接建在 BP_SurvivalGamemode 里，所以客户端永远看不到。
	GameStateClass = AWarriorSurvialGameState::StaticClass();
}

AWarriorSurvialGameState* AWarriorSurvialGameMode::GetSurvialGameState() const
{
	return GetGameState<AWarriorSurvialGameState>();
}

void AWarriorSurvialGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetCurrentState(ESurvivalGameModeState::WaitSpawnNewWave);

	// 数据表必须先在 GameMode 蓝图里指派，否则直接跳过波次初始化（避免崩溃）
	if (!EnemyWaveSpawnerDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("AWarriorSurvialGameMode::BeginPlay: EnemyWaveSpawnerDataTable 未设置，波次系统未启动"));
		return;
	}

	TotalWaveToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	// 同步给 GameState：客户端 UI 要显示"第几波 / 共几波"和倒计时时长，
	// 而这些配置在 GameMode 上（客户端读不到），所以一并复制过去。
	if (AWarriorSurvialGameState* SurvialGameState = GetSurvialGameState())
	{
		SurvialGameState->SetWaveTimings(SpawnWaitTime, SpawnEnemyDelayTime, WaveCompletedWaitTime);
		SurvialGameState->SetWaveInfo(CurrentWaveCount, TotalWaveToSpawn);
	}

	PreLoadNextWaveEnemies();
}

void AWarriorSurvialGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (CurrentGameModeState == ESurvivalGameModeState::WaitSpawnNewWave)
	{
		TimePassSinceStart += DeltaSeconds;
		if (TimePassSinceStart >= SpawnWaitTime)
		{
			TimePassSinceStart = 0.f;

			SetCurrentState(ESurvivalGameModeState::SpawningNewWave);
		}
	}

	if (CurrentGameModeState == ESurvivalGameModeState::SpawningNewWave)
	{
		TimePassSinceStart += DeltaSeconds;
		if (TimePassSinceStart>=SpawnEnemyDelayTime)
		{
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
			
			TimePassSinceStart = 0.f;
			
			SetCurrentState(ESurvivalGameModeState::InProgress);
		}
	}

	if (CurrentGameModeState == ESurvivalGameModeState::InProgress)
	{
		
	}

	if (CurrentGameModeState == ESurvivalGameModeState::WaveComplete)
	{
		TimePassSinceStart += DeltaSeconds;
		if (TimePassSinceStart >= WaveCompletedWaitTime)
		{
			TimePassSinceStart = 0.f;

			CurrentWaveCount++;

			// 同步波次信息给 GameState（客户端 UI 读的是 GameState）
			if (AWarriorSurvialGameState* SurvialGameState = GetSurvialGameState())
			{
				SurvialGameState->SetWaveInfo(CurrentWaveCount, TotalWaveToSpawn);
			}

			if (HasFinishedAllWaves())
			{
				SetCurrentState(ESurvivalGameModeState::AllWaveDone);
			}
			else
			{
				SetCurrentState(ESurvivalGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void AWarriorSurvialGameMode::SetCurrentState(ESurvivalGameModeState NewState)
{
	CurrentGameModeState = NewState;

	// 服务器内部广播（仅为兼容既有的 BP_SurvivalGamemode 绑定而保留）
	OnCurrentGameStateChange.Broadcast(CurrentGameModeState);

	// 同步到 GameState：客户端靠它收到状态变化并驱动 UI
	//（放在 GameMode 上的委托客户端连对象都没有，绑不上 —— 这就是"UI 只在服务器可见"的根因）
	if (AWarriorSurvialGameState* SurvialGameState = GetSurvialGameState())
	{
		// 计时配置也一起同步：BP 里会在 SpawningNewWave 分支运行时改 SpawnEnemyDelayTime，
		// 只同步一次的话客户端会一直用旧值显示倒计时。
		SurvialGameState->SetWaveTimings(SpawnWaitTime, SpawnEnemyDelayTime, WaveCompletedWaitTime);
		SurvialGameState->SetWaveInfo(CurrentWaveCount, TotalWaveToSpawn);
		SurvialGameState->SetCurrentGameModeState(NewState);
	}
}

bool AWarriorSurvialGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWaveToSpawn;
}

void AWarriorSurvialGameMode::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves()) return;

	PreloadedEnemyClassMap.Empty();
	for (const FWarriorEnemyWaveSpawnerInfo& EnemyWaveSpawnerInfo : GetCurrentWaveSpawnerTableRow().EnemyWaveSpawnerInfos)
	{
		if (EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		UAssetManager::GetStreamableManager().RequestAsyncLoad(EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([EnemyWaveSpawnerInfo,this]()
		{
			if (UClass* LoadedEnemyClass = EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn.Get())
			{
				PreloadedEnemyClassMap.Emplace(EnemyWaveSpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
			}
		}
		));
	}
	
}

int32 AWarriorSurvialGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this,ATargetPoint::StaticClass(),TargetPointsArray);
	}

	uint32 EnemiesSpawned = 0;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow().EnemyWaveSpawnerInfos)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		UClass*	EnemyClassToSpawn = PreloadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i = 0; i < NumToSpawn; i++)
		{
			const int32 RandomIndex = FMath::RandRange(0, SpawnerInfo.MaxPerSpawnCount-1);
			const FVector SpawnOrigin = TargetPointsArray[RandomIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomIndex]->GetActorForwardVector().ToOrientationRotator();
			FVector RandomSpawnLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin,RandomSpawnLocation , 300.f);

			RandomSpawnLocation += FVector(0.f,0.f,150.f);
			AWarriorEnemyCharacter*  SpawnedEnemy = GetWorld()->SpawnActor<AWarriorEnemyCharacter>(EnemyClassToSpawn, RandomSpawnLocation, SpawnRotation,SpawnInfo);

			if (SpawnedEnemy)
			{
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&AWarriorSurvialGameMode::OnEnemyDestroy);
				EnemiesSpawned++;
				TotalSpawnThisWaveCounter++;
			}

			if (!ShouldSpawnEnemies())
			{
				return EnemiesSpawned;
			}
		}
	}
	return EnemiesSpawned;
}

FWarriorEnemyWaveSpawnerTableRow AWarriorSurvialGameMode::GetCurrentWaveSpawnerTableRow() const
{
	if (!EnemyWaveSpawnerDataTable)
	{
		return FWarriorEnemyWaveSpawnerTableRow();
	}
	
	const FName RowName(*FString::Printf(TEXT("Wave%d"), CurrentWaveCount));
	const FWarriorEnemyWaveSpawnerTableRow* FoundRow =
		EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, TEXT("GetCurrentWaveSpawnerTableRow"), false);
	
	if (!FoundRow)
	{
		return FWarriorEnemyWaveSpawnerTableRow();
	}

	return *FoundRow;
}

bool AWarriorSurvialGameMode::ShouldSpawnEnemies() const
{
	return TotalSpawnThisWaveCounter < GetCurrentWaveSpawnerTableRow().TotalEnemyToSpawnThisWave;
}

void AWarriorSurvialGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	EWarriorGameDifficulty CurrentDifficulty;
	if (UWarriorFunctionLibrary::TryLoadSaveCurrentGameDifficulty(CurrentDifficulty))
	{
		CurrentGameDifficulty = CurrentDifficulty;
	}

	
}

void AWarriorSurvialGameMode::OnEnemyDestroy(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;

	Debug::Print(TEXT("OnEnemyDestroy: 当前存活敌人数量 %d"), CurrentSpawnedEnemiesCounter);
	if (ShouldSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		TotalSpawnThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;
		SetCurrentState(ESurvivalGameModeState::WaveComplete);
	}
}

void AWarriorSurvialGameMode::RegisterSpawnedEnemy(const TArray<AWarriorEnemyCharacter*>& InEnemies)
{
	for (AWarriorEnemyCharacter* Enemy : InEnemies)
	{
		if (Enemy)
		{
			Enemy->OnDestroyed.AddUniqueDynamic(this, &AWarriorSurvialGameMode::OnEnemyDestroy);
			CurrentSpawnedEnemiesCounter++;
		}
	}
}
