// MyRPGDemo


#include "Widget/Components/FrontendCommonRotator.h"

#include "CommonTextBlock.h"

void UFrontendCommonRotator::SetSelectedOptionByText(const FText& InTextOption)
{
	const int32 FoundIndex = TextLabels.IndexOfByPredicate([InTextOption](const FText& TextLabel)->bool
	{
		return TextLabel.EqualTo(InTextOption);
	});

	if (FoundIndex != INDEX_NONE)
	{
		SetSelectedItem(FoundIndex);
	}
	else
	{
		MyText->SetText(InTextOption);
	}
}
