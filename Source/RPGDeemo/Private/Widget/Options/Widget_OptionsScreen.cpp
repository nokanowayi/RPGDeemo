// MyRPGDemo


#include "Widget/Options/Widget_OptionsScreen.h"

#include "ICommonInputModule.h"
#include "WarriorDebugHelper.h"
#include "Input/CommonUIInputTypes.h"
#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Settings/FrontendGameUserSettings.h"


void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(FBindUIActionArgs(ResetAction,
	    true,
    	FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggered)
	        )
        );
	}
	
	RegisterUIActionBinding(FBindUIActionArgs(ICommonInputModule::GetSettings().GetDefaultBackAction(),
		true,
		FSimpleDelegate::CreateUObject(this, &ThisClass::OnBackBoundActionTriggered)));

	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this, &ThisClass::OnOptionsTabSelected);
	
	CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this, &ThisClass::OnListViewItemHovered);
	CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnListViewItemSelected);
}

void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	for (UListDataObject_Collection* TabCollections : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollections) continue;

		if(TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabCollections->GetDataID()) != nullptr)
		{
			continue;
		}

		TabListWidget_OptionsTabs->RequestRegisterTab(TabCollections->GetDataID(),TabCollections->GetDisplayName());
	}
}

void UWidget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	UFrontendGameUserSettings::Get()->ApplySettings(true);
}

UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateDataRegistry()
{
	if (!OwningDataRegistry)
	{
		OwningDataRegistry = NewObject<UOptionsDataRegistry>();
		OwningDataRegistry->InitUOptionsDataRegistry(GetOwningLocalPlayer());
	}

	checkf(OwningDataRegistry,TEXT("UWidget_OptionsScreen::GetOrCreateDataRegistry() - OwningDataRegistry is null after creation!"));
	return OwningDataRegistry;
}

void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("Reset Action Triggered"), 5.f, FColor::Red);
}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabId)
{
	TArray<UListDataObject_Base*> FoundItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabId);

	CommonListView_OptionsList->SetListItems(FoundItems);
	CommonListView_OptionsList->RequestRefresh();
	CommonListView_OptionsList->NavigateToIndex(0);
	CommonListView_OptionsList->SetSelectedIndex(0);
}

void UWidget_OptionsScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem)
	{
		return;
	}
	UWidget_ListEntry_Base* HoverEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem<UWidget_ListEntry_Base>(InHoveredItem);

	if (HoverEntryWidget)
	{
		HoverEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);
	}
}

void UWidget_OptionsScreen::OnListViewItemSelected(UObject* InHoveredItem)
{
	if (!InHoveredItem)
	{
		return;
	}
}
