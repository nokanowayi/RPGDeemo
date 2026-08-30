// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "Component/Combat/EnemyCombatComponent.h"
#include "Component/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "WarriorEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	AWarriorEnemyCharacter();

	virtual  UPawnCombatComponent* GetPawnCombatComponent() const override;
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;
protected:
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	/** 尝试绑定敌人血条 widget（WidgetComponent 的 widget 可能晚于 BeginPlay 创建，延迟到下一帧/重试） */
	void TryBindEnemyHealthWidget();
	
	/** 血条绑定重试次数 */
	int32 HealthWidgetBindRetryCount = 0;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "UI")
	UEnemyUIComponent* EnemyUIComponent;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* EnemyHealthWidgetComponent;
	private:
	void InitEnemyStartUpData();
public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const {return EnemyCombatComponent; };
};
