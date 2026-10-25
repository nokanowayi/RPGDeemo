// MyRPGDemo


#include "Widget/Options/Widget_OptionsScreen.h"

#include "ICommonInputModule.h"
#include "WarriorDebugHelper.h"
#include "Input/CommonUIInputTypes.h"
#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Settings/FrontendGameUserSettings.h"
#include "Subsystems/FrontendUISubsystem.h"


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
	if (ResettableDataArray.IsEmpty())
	{
		return;
	}
	

	UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		EConfirmScreenType::YesNo,
		FText::FromString(TEXT("Reset")),
		FText::FromString(TEXT("Are you sure you want to reset all options to their default values?")),
		[this](EConfirmScreenButtonType InButtonType)
		{
			if (InButtonType != EConfirmScreenButtonType::Confirmed)
			{
				return;
			}

			bIsResetting = true;
			bool bHasDataFailedToReset = false;
			for (UListDataObject_Base* TabCollection : ResettableDataArray)
			{
				if (!TabCollection) continue;
				if(TabCollection->TryResetBackToDefaultValue())
				{
					Debug::Print(FString::Printf(TEXT("Reset %s to default value."),*TabCollection->GetDisplayName().ToString()),5.f,FColor::Green);
				}
				else
				{
					bHasDataFailedToReset = true;
					Debug::Print(FString::Printf(TEXT("Failed to reset %s to default value."),*TabCollection->GetDisplayName().ToString()),5.f,FColor::Red);
				}
			}

			if (!bHasDataFailedToReset)
			{
				ResettableDataArray.Empty();
                RemoveActionBinding(ResetActionHandle);
			}

			bIsResetting = false;
		});

	for (UListDataObject_Base* ResettableData : ResettableDataArray)
	{
		if (!ResettableData) continue;

		if (ResettableData->CanResetBackToDefaultValue())
		{
			ResettableData->TryResetBackToDefaultValue();
		}
	}
}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabId)
{
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();
	TArray<UListDataObject_Base*> FoundItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabId);

	CommonListView_OptionsList->SetListItems(FoundItems);
	CommonListView_OptionsList->RequestRefresh();
	CommonListView_OptionsList->NavigateToIndex(0);
	CommonListView_OptionsList->SetSelectedIndex(0);

	ResettableDataArray.Empty();
	for (UListDataObject_Base* FoundItem : FoundItems)
	{
		if (!FoundItem) continue;
		if (FoundItem->CanResetBackToDefaultValue())
		{
			ResettableDataArray.Add(FoundItem);
		}

		if (!FoundItem->OnListDataModified.IsBoundToObject(this))
		{
			FoundItem->OnListDataModified.AddUObject(this,&ThisClass::OnListViewDataModified);
		}
		if (ResettableDataArray.IsEmpty())
		{
			RemoveActionBinding(ResetActionHandle);
		}
		else
		{
			if (!GetActionBindings().Contains(ResetActionHandle))
			{
				RemoveActionBinding(ResetActionHandle);
			}
		}
	}
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

		if (bWasHovered)
		{
			DetailsView_ListEntryInfo->UpdateDetailsViewInfo(Cast<UListDataObject_Base>(InHoveredItem),
				TryGetEntryWidgetClassName(InHoveredItem));
		}
		else
		{
			if (UListDataObject_Base* SelectItem = CommonListView_OptionsList->GetSelectedItem<UListDataObject_Base>())
			{
				DetailsView_ListEntryInfo->UpdateDetailsViewInfo(SelectItem,TryGetEntryWidgetClassName(InHoveredItem));
			}
		}
	}
}

void UWidget_OptionsScreen::OnListViewItemSelected(UObject* InHoveredItem)
{
	if (!InHoveredItem)
	{
		return;
	}
	
	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(Cast<UListDataObject_Base>(InHoveredItem),
		TryGetEntryWidgetClassName(InHoveredItem));
}

FString UWidget_OptionsScreen::TryGetEntryWidgetClassName(UObject* InOwningListItem) const
{
	if (UUserWidget* Found = CommonListView_OptionsList->GetEntryWidgetFromItem(InOwningListItem))
	{
		return Found->GetClass()->GetName();
	}
	return FString();
}

void UWidget_OptionsScreen::OnListViewDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifiedReason)
{

	if (bIsResetting || !ModifiedData)
	{
		return;
	}
	
	if (ModifiedData->CanResetBackToDefaultValue())
	{
		ResettableDataArray.AddUnique(ModifiedData);
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
	else
	{
		if (ResettableDataArray.Contains(ModifiedData))
		{
			ResettableDataArray.Remove(ModifiedData);
		}
	}
	
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
}
