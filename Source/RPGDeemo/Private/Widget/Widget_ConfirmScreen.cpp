// MyRPGDemo


#include "Widget/Widget_ConfirmScreen.h"

#include "ICommonInputModule.h"
#include "Widget/Components/FrontendCommonButtonBase.h"

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOKScreen(const FText& InScreenTitle,
                                                                   const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo ButtonInfo;
	ButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	ButtonInfo.ButtonText = FText::FromString(TEXT("OK"));
	InfoObject->AvailableScreenButtons.Add(ButtonInfo);

	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateYesNoScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo YesButtonInfo;
	YesButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	YesButtonInfo.ButtonText = FText::FromString(TEXT("Yes"));
	InfoObject->AvailableScreenButtons.Add(YesButtonInfo);

	FConfirmScreenButtonInfo NoButtonInfo;
	NoButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	NoButtonInfo.ButtonText = FText::FromString(TEXT("No"));
	InfoObject->AvailableScreenButtons.Add(NoButtonInfo);

	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOKCancelScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo ButtonInfo;
	ButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	ButtonInfo.ButtonText = FText::FromString(TEXT("OK"));
	InfoObject->AvailableScreenButtons.Add(ButtonInfo);
	
	FConfirmScreenButtonInfo CancelButtonInfo;
	CancelButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	CancelButtonInfo.ButtonText = FText::FromString(TEXT("Cancel"));
	InfoObject->AvailableScreenButtons.Add(CancelButtonInfo);
	
	return InfoObject;
}

void UWidget_ConfirmScreen::InitConfirmScreen(UConfirmScreenInfoObject* InConfirmScreenInfoObject,
	TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback)
{
	CommonTextBlock_Title->SetText(InConfirmScreenInfoObject->ScreenTitle);
	CommonTextBlock_Message->SetText(InConfirmScreenInfoObject->ScreenMessage);

	if (DynamicEntryBox_Buttons->GetNumEntries()!=0)
	{
		DynamicEntryBox_Buttons->Reset<UFrontendCommonButtonBase>([](UFrontendCommonButtonBase& ExistingButton)
		{
			ExistingButton.OnClicked().Clear();
		});
	}

	for (const FConfirmScreenButtonInfo& ButtonInfo : InConfirmScreenInfoObject->AvailableScreenButtons)
	{
		FDataTableRowHandle InputActionHandle;
		switch (ButtonInfo.ConfirmScreenButtonType)
		{
		case EConfirmScreenButtonType::Cancelled:
			InputActionHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;
		case EConfirmScreenButtonType::Closed:
			InputActionHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;
			default:
			break;
		}
		
		UFrontendCommonButtonBase* NewButton = DynamicEntryBox_Buttons->CreateEntry<UFrontendCommonButtonBase>();
		NewButton->SetButtonText(ButtonInfo.ButtonText);
		NewButton->SetTriggeringInputAction(InputActionHandle);
		NewButton->OnClicked().AddLambda([ClickedButtonCallback, ButtonInfo,this]()
		{
			if (ClickedButtonCallback)
			{
				ClickedButtonCallback(ButtonInfo.ConfirmScreenButtonType);

				DeactivateWidget();
			}
		});

		if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
		{
			DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus();
		}
	}
}
