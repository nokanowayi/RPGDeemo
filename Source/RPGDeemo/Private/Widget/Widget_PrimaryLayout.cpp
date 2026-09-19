// MyRPGDemo


#include "Widget/Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InStackTag) const
{
	// 不要用 checkf：BP 传空 tag / 忘了调 RegisterWidgetStack 都是常见失误，
	// 直接断言会把整个编辑器一起带走（刚刚就崩过一次），这里改成"报错但不死"，由调用方决定怎么办
	if (!InStackTag.IsValid())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Widget_PrimaryLayout] FindWidgetStackByTag: 传入的 StackTag 是空的（蓝图上的 GameplayTag 引脚没设值）"));
		return nullptr;
	}

	if (UCommonActivatableWidgetContainerBase* const* FoundStack = RegisteredWidgetStackMap.Find(InStackTag))
	{
		return *FoundStack;
	}

	// 用了 ensure 而不是 check：开发期会大声报错（挂调试器时会断下来），但不会崩进程
	ensureMsgf(false, TEXT("[Widget_PrimaryLayout] Widget stack [%s] 没有注册过（蓝图里忘了对该 Stack 控件调用 RegisterWidgetStack？）"), *InStackTag.ToString());
	return nullptr;
}

void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "UI.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	if (!IsDesignTime())
	{
		if (!RegisteredWidgetStackMap.Contains(InStackTag))
		{
			RegisteredWidgetStackMap.Add(InStackTag, InStack);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget stack with tag %s is already registered."), *InStackTag.ToString());
		}
	}
}
