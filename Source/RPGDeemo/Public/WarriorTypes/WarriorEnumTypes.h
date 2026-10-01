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