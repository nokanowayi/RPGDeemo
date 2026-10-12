// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "WarriorGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FWarriorGameLevelSet
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,meta = (Categories = "GameData.Level"))
	FGameplayTag LevelTag;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Level;

	bool IsValid() const
	{
		return LevelTag.IsValid() && !Level.IsNull();
	}
};
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	// 必须解绑 Init 里挂到全局 FCoreUObjectDelegates 上的回调，
	// 否则全局委托会一直强引用本 GameInstance（World/控件/子系统一起漏，退出 PIE 会报"仍被引用"）
	virtual void Shutdown() override;
protected:
	virtual void OnPreLoadMap(const FString& MapName);
	virtual void OnDestinationWorldLoaded(UWorld* LoadedWorld);
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FWarriorGameLevelSet> GameLevelSets;
public:
	UFUNCTION(BlueprintCallable,meta = (GameplayTagFilter = "GameData.Level"))
	TSoftObjectPtr<UWorld> GetLevelByTag(FGameplayTag LevelTag);
};
