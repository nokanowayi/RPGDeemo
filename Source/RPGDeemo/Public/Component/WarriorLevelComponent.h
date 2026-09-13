// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Components/ActorComponent.h"
#include "WarriorLevelComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGDEEMO_API UWarriorLevelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWarriorLevelComponent();

	UFUNCTION(BlueprintCallable, Category="Level")
	void GainExp(int32 InExp);

	UFUNCTION(BlueprintCallable, Category="Level")
	void SettleExp();
	
	UFUNCTION(BlueprintCallable, Category="Level")
	void InitializeStats();
	
	UFUNCTION(BlueprintPure, Category="Level")
	float GetExpToNextLevel(int32 Level) const
	{
		return FMath::Max(0.f, ExpToNextLevel.GetValueAtLevel(static_cast<float>(Level)));
	}
protected:
	
	void SettleExpInternal(int32 InExp);
	UPROPERTY(EditDefaultsOnly, Category="Level")
	FScalableFloat ExpToNextLevel;

	void ApplyLevelGrowth(int32 NewLevel);

	UPROPERTY(EditDefaultsOnly, Category="Level")
	TSubclassOf<UGameplayEffect> LevelUpStatsEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Level")
	TSubclassOf<UGameplayEffect> FillStatsEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Level")
	int32 MaxLevel = 5;
		
};
