#pragma once

UENUM(BlueprintType)
enum class EWarriorConfirmType : uint8 { Yes, No };

UENUM(BlueprintType)
enum class EWarriorValidType : uint8 { Valid, Invalid };

UENUM(BlueprintType)
enum class EWarriorSuccessType : uint8 { Successful, Failed };

UENUM(BlueprintType)
enum class EWarriorCountDownActionInput : uint8
{
	Start,
	Cancel
};

UENUM(BlueprintType)
enum class EWarriorCountDownActionOutput : uint8
{
	Updated,
	Completed,
	Cancelled
};

UENUM(BlueprintType)
enum class EWarriorGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	VeryHard
};

UENUM(BlueprintType)
enum class EWarriorInputMode : uint8
{
	GameOnly,
	UIOnly
};

UENUM(BlueprintType)
enum class EConfirmScreenType : uint8
{
	OK,
	YesNo,
	OKCancel,
	Unknown UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EConfirmScreenButtonType : uint8
{
	Confirmed,
	Cancelled,
	Closed,
	Unknown UMETA(Hidden)
};

/**
 * 生存模式的状态机状态。
 *
 * 注意：这个枚举原来定义在 WarriorSurvialGameMode.h 里，现在挪到这里 ——
 * 因为 AWarriorSurvialGameState 也要用它，而 GameState 与 GameMode 不能互相 include。
 * 枚举在蓝图里的引用是按"模块 + 枚举名"解析的，所以挪动文件不会让已有蓝图节点失效。
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