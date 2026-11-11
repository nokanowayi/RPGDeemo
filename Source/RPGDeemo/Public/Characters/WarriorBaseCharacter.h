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
	
	virtual void OnRep_PlayerState() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	virtual void OnRep_AbilitySystemInitialized();
	
	void InitializeAbilitySystemForClient();
	
	void ActivateOnGivenAbilitiesForClient();
	
	bool TryActivateOnGivenAbilities();
	
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

	UFUNCTION(blueprintCallable, Category = "Level")
	void OpenLobby();

	UFUNCTION(blueprintCallable, Category = "Level")
	void CallOpenLevel(const FString& LevelName);

	UFUNCTION(blueprintCallable, Category = "Level")
	void CallClientTravel(const FString& LevelName);
};
