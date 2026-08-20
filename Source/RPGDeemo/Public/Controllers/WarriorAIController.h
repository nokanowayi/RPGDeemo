// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "WarriorAIController.generated.h"

class UAISenseConfig_Sight;

/**
 * 
 */
UCLASS()
class RPGDEEMO_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()
public:
	AWarriorAIController(const class FObjectInitializer& ObjectInitializer);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;

	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

private:
	UPROPERTY(EditDefaultsOnly,Category = "Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance = true;

	UPROPERTY(EditDefaultsOnly,Category = "Detour Crowd Avoidance Config",meta = (EditCondition = "bEnableDetourCrowdAvoidance",UIMin = "1",UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;

	UPROPERTY(EditDefaultsOnly,Category = "Detour Crowd Avoidance Config",meta = (EditCondition = "bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};
