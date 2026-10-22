// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "DataObject/ListDataObject_Collection.h"
#include "UObject/NoExportTypes.h"
#include "OptionsDataRegistry.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEEMO_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:
	void InitUOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }

	TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const;
private:
	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab();

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;
};
