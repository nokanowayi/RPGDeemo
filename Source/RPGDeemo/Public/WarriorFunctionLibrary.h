// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "GameInstance/WarriorGameInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "Widget/Widget_ActivatableBase.h"
#include "WarriorFunctionLibrary.generated.h"

class UPawnCombatComponent;


/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static UWarriorAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor,FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor,FGameplayTag TagToAdd);

	static bool NativeDoesActorHaveTag(AActor* InActor,FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary",meta = (DisplayName = "Does Actor Have Tag",ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor,FGameplayTag TagToCheck,EWarriorConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary",meta = (DisplayName = "Get Pawn Combat Component",ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor,EWarriorValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary",meta = (CompactNodeTitle = "Get Value At Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, int32 InLevel = 1);

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirTag(AActor* InAttacker,AActor* InVictim,float& OutAngelDifference);

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static bool IsValidBlock(AActor* InAttacker,AActor* InDefender);

	UFUNCTION(Blueprintcallable, Category = "Warrior|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator,AActor* InTargetActor,const FGameplayEffectSpecHandle InSpecHandle);
	
	UFUNCTION(Blueprintcallable, Category = "Warrior|FunctionLibrary",meta = (Latent,WorldContext = "WorldContextObject",LatentInfo = "LatentInfo",ExpandEnumAsExecs = "CountDownInput|CountDownOutput",TotalTime = "1.0",UpdateInterval = "0.1"))
	static void CountDown(const UObject* WorldContextObject, float TotalTime,float UpdateInterval, float& OutRemainingTime,
		UPARAM(DisplayName = "OutPut") EWarriorCountDownActionInput CountDownInput,EWarriorCountDownActionOutput& CountDownOutput,FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary",meta = (WorldContext = "WorldContextObject"))
	static UWarriorGameInstance* GetGameInstance(const UObject* WorldContextObject);

	UFUNCTION(Blueprintcallable, Category = "Warrior|FunctionLibrary",meta = (WorldContext = "WorldContextObject"))
	static void ToogleInputMode(EWarriorInputMode InputMode,const UObject* WorldContextObject);

	UFUNCTION(Blueprintcallable, Category = "Warrior|FunctionLibrary")
	static void SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficulty);
	
	UFUNCTION(Blueprintcallable, Category = "Warrior|FunctionLibrary")
	static bool TryLoadSaveCurrentGameDifficulty(EWarriorGameDifficulty& OutDifficulty);
	
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static TSoftClassPtr<UWidget_ActivatableBase> GetFrontendWidgetClassByTag(UPARAM (meta = (Categories = "UI.Widget"))FGameplayTag InTag);
};
