// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGDEEMO_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

	protected:
	template<class T>
	T* GetOwningPawn() const
	{
		return Cast<T>(GetOwner());
	}

	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	template<class T>
	T* GetOwningController() const
	{
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
