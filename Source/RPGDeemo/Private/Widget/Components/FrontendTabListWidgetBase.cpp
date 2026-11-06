// MyRPGDemo


#include "Widget/Components/FrontendTabListWidgetBase.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif

// 运行时也要用（Widget_OptionsScreen::NativeOnActivated 里注册 Tab），不能放在 WITH_EDITOR 里
void UFrontendTabListWidgetBase::RequestRegisterTab(const FName& TabID, const FText& TabDisplayName)
{
	RegisterTab(TabID, TabButtonEntryWidgetClass, nullptr, -1);

	if (UFrontendCommonButtonBase* FoundButton = Cast<UFrontendCommonButtonBase>(GetTabButtonBaseByID(TabID)))
	{
		FoundButton->SetButtonText(TabDisplayName);
	}
}

#if WITH_EDITOR
void UFrontendTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(TEXT("TabButtonEntryWidgetClass is not set. Please set a valid class for the tab button entry widget.")));
	}
}
#endif
