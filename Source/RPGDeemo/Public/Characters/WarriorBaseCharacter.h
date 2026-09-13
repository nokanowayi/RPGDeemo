// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Component/WarriorLevelComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "GameFramework/Character.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "WarriorBaseCharacter.generated.h"


class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;
class UMotionWarpingComponent;

UCLASS()
class RPGDEEMO_API AWarriorBaseCharacter : public ACharacter,public IAbilitySystemInterface,public IPawnCombatInterface,public IPawnUIInterface
{
	GENERATED_BODY()

public:
	AWarriorBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual  UPawnCombatComponent* GetPawnCombatComponent() const override;

	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual  UWarriorLevelComponent* GetWarriorLevelComponent() const;

protected:
	virtual void PossessedBy(AController* NewController) override;

	/** 客户端复制 PlayerState 后初始化 ASC（玩家用） */
	virtual void OnRep_PlayerState() override;

	/** 复制注册（属性复制声明） */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 客户端收到"服务器已初始化ASC"标记后，初始化本端 ASC（敌人无PlayerState，靠这个） */
	UFUNCTION()
	virtual void OnRep_AbilitySystemInitialized();

	/** 客户端初始化 ASC（玩家走 OnRep_PlayerState，敌人走 OnRep_AbilitySystemInitialized） */
	void InitializeAbilitySystemForClient();

	/** 客户端 ASC 初始化后，激活 OnGiven 策略的能力（如 HUD 创建） */
	void ActivateOnGivenAbilitiesForClient();

	/** 尝试激活 OnGiven 能力，返回是否找到 */
	bool TryActivateOnGivenAbilities();

	/** OnGiven 能力重试计数（等能力列表复制完成） */
	int32 OnGivenRetryCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;

	/** 服务器初始化ASC后置true并复制，客户端据此初始化本端ASC */
	UPROPERTY(ReplicatedUsing = OnRep_AbilitySystemInitialized)
	bool bAbilitySystemInitialized = false;
public:
	UFUNCTION(BlueprintCallable, Category = "Ability")
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const { return WarriorAbilitySystemComponent; }
	UFUNCTION(BlueprintCallable, Category = "Ability")
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const { return WarriorAttributeSet; }
};
