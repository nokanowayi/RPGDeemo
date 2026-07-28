// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriHeroLinkedAnimLayer.generated.h"

class  UWarriorHeroAnimInstance;
/**
 * 
 */
UCLASS()
class RPGDEEMO_API UWarriHeroLinkedAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(blueprintCallable,meta = (BlueprintThreadSafe))
	UWarriorHeroAnimInstance* GetHeroAnimInstance() const;
};
