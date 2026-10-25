// MyRPGDemo


#include "Widget/Options/WidgetOptionDetailsView.h"

void UWidgetOptionDetailsView::UpdateDetailsViewInfo(UListDataObject_Base* InDataObject,
	const FString& InEntryWidgetClassName)
{
	if (!InDataObject) return;

	CommonTextBlock_Title->SetText(InDataObject->GetDisplayName());
	if (!InDataObject->GetSoftDescriptionImage().IsNull())
	{
		CommonLazyImage_DescriptionImage->SetBrushFromLazyTexture(InDataObject->GetSoftDescriptionImage());
		CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	CommonRichTextBlock_Description->SetText(InDataObject->GetDescriptionRichText());
	const FString DynamicDetailsText = FString::Printf(TEXT("Data Object Class: <Bold> %s</>\n\nEntry Widget Class: <Bold> %s</>"),*InDataObject->GetClass()->GetName() ,*InEntryWidgetClassName);
	CommonRichTextBlock_DynamicDetails->SetText(FText::FromString(DynamicDetailsText));
	CommonRichTextBlock_DisabledReason->SetText(InDataObject->GetDisabledRichText());
}

void UWidgetOptionDetailsView::ClearDetailsViewInfo()
{
	CommonTextBlock_Title->SetText(FText::GetEmpty());
	CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::Collapsed);
	CommonRichTextBlock_Description->SetText(FText::GetEmpty());
	CommonRichTextBlock_DynamicDetails->SetText(FText::GetEmpty());
	CommonRichTextBlock_DisabledReason->SetText(FText::GetEmpty());
}

void UWidgetOptionDetailsView::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ClearDetailsViewInfo();
}
