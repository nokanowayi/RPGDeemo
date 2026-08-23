// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Component/PawnExtensionComponentBase.h"
#include "PawnUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChangedDelegate,float,NewPercent);
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UPawnUIComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

	public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentHealthChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Warrior|UI")
	void BroadcastCurrentHealthState();
};
