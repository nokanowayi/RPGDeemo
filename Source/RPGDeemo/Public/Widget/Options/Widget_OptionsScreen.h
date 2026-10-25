// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "OptionsDataRegistry.h"
#include "WidgetOptionDetailsView.h"
#include "Widget/Widget_ActivatableBase.h"
#include "Widget/Components/FrontendCommonListView.h"
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

	virtual void NativeOnDeactivated() override;
	

	
private:
	UOptionsDataRegistry* GetOrCreateDataRegistry();
	
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();

	UFUNCTION()
	void OnOptionsTabSelected(FName TabId);

	void OnListViewItemHovered(UObject* InHoveredItem,bool bWasHovered);
	void OnListViewItemSelected(UObject* InHoveredItem);

	FString TryGetEntryWidgetClassName(UObject* InOwningListItem) const;

	void OnListViewDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifiedReason);
	
	UPROPERTY(meta=(BindWidget))
	UFrontendTabListWidgetBase* TabListWidget_OptionsTabs;
	UPROPERTY(meta=(BindWidget))
	UWidgetOptionDetailsView* DetailsView_ListEntryInfo;
	UPROPERTY(meta=(BindWidget))
	UFrontendCommonListView* CommonListView_OptionsList;
	UPROPERTY(transient)
	UOptionsDataRegistry* OwningDataRegistry;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	

	FUIActionBindingHandle ResetActionHandle;
	
	UPROPERTY(transient)
	TArray<UListDataObject_Base*> ResettableDataArray;

	bool bIsResetting = false;
};

