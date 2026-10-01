// MyRPGDemo


#include "Widget/Components/FrontendTabListWidgetBase.h"

#include "Editor/WidgetCompilerLog.h"

#if WITH_EDITOR	
void UFrontendTabListWidgetBase::RequestRegisterTab(const FName& TabID, const FText& TabDisplayName)
{
	RegisterTab(TabID, TabButtonEntryWidgetClass, nullptr,-1);

	if (UFrontendCommonButtonBase* FoundButton = Cast<UFrontendCommonButtonBase>( GetTabButtonBaseByID(TabID)))
	{
		FoundButton->SetButtonText(TabDisplayName);
	}
}

void UFrontendTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(TEXT("TabButtonEntryWidgetClass is not set. Please set a valid class for the tab button entry widget.")));
	}
}
#endif
