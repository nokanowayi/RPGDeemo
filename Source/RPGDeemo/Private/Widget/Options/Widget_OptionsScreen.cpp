// MyRPGDemo


#include "Widget/Options/Widget_OptionsScreen.h"

#include "ICommonInputModule.h"
#include "WarriorDebugHelper.h"
#include "Input/CommonUIInputTypes.h"
#include "AsyncActions/AsyncAction_PushSoftWidget.h"


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
	Debug::Print(FString::Printf(TEXT("Options Tab Selected: %s"), *TabId.ToString()), 5.f, FColor::Green);
}
