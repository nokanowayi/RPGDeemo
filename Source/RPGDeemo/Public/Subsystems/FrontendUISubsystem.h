// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WarriorGameplayTags.h"
#include "Widget/Widget_ActivatableBase.h"
#include "Widget/Widget_PrimaryLayout.h"
#include "FrontendUISubsystem.generated.h"

enum class EAsyncPushWidgetState:uint8
{
	OnCreatedBeforePush,
	AfterPush
};
UCLASS()
class RPGDEEMO_API UFrontendUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UFrontendUISubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InPrimaryLayoutWidget);

	void PushSoftWidgetToStackAsync(const FGameplayTag& InStackTag,TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,TFunction<void(EAsyncPushWidgetState,UWidget_ActivatableBase*)> AsyncPushStateCallBack);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	UWidget_PrimaryLayout* GetPrimaryLayoutWidget() const { return CreatedPrimaryLayoutWidget; }

private:
	UPROPERTY(transient)
	UWidget_PrimaryLayout* CreatedPrimaryLayoutWidget = nullptr;
};
