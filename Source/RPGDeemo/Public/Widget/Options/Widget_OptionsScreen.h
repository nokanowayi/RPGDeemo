// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "OptionsDataRegistry.h"
#include "Widget/Widget_ActivatableBase.h"
#include "Widget/Components/FrontendTabListWidgetBase.h"
#include "Widget_OptionsScreen.generated.h"

/**
 * 
 */
UCLASS(Abstract,BlueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()


protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnActivated() override;

	UPROPERTY(meta=(BindWidget))
	UFrontendTabListWidgetBase* TabListWidget_OptionsTabs;
	
private:
	UOptionsDataRegistry* GetOrCreateDataRegistry();
	
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();

	UFUNCTION()
	void OnOptionsTabSelected(FName TabId);

	UPROPERTY(transient)
	UOptionsDataRegistry* OwningDataRegistry;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	

	FUIActionBindingHandle ResetActionHandle;
};

