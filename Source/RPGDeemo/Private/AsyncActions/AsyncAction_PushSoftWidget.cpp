// MyRPGDemo


#include "AsyncActions/AsyncAction_PushSoftWidget.h"

#include "Engine/Engine.h"   // GEngine / EGetWorldErrorMode：单独编译本文件时不能靠 unity 里邻居的 include
#include "Subsystems/FrontendUISubsystem.h"

UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidgetToStackAsync(UObject* WorldContextObject,
                                                                                     UPARAM(meta = (Categories = "UI.WidgetStack"))FGameplayTag InStackTag, APlayerController* InPlayerController,
                                                                                     TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, bool bFocusOnNewlyPushedWidget)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();
			Node->CachedOwingWorld = World;
			Node->CachedOwingPC = InPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedStackTag = InStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;
			
			Node->RegisterWithGameInstance(World);

			return Node;
		}
	}

	return nullptr;
}

void UAsyncAction_PushSoftWidget::Activate()
{
	UFrontendUISubsystem* FrontendUISubsystem = UFrontendUISubsystem::Get(CachedOwingWorld.Get());

	// 世界/子系统取不到就别往下走：这里空指针解引用崩起来比"UI 没弹出来"难查得多
	if (!FrontendUISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AsyncAction_PushSoftWidget] 取不到 UFrontendUISubsystem（World 已失效或子系统未创建），本次推送取消"));
		SetReadyToDestroy();
		return;
	}

	FrontendUISubsystem->PushSoftWidgetToStackAsync(CachedStackTag, CachedSoftWidgetClass, [this](EAsyncPushWidgetState InState, UWidget_ActivatableBase* PushedWidget)
	{
		// AddWidget 在"类不是 UWidget_ActivatableBase 子类"时会返回 nullptr，别直接解引用
		if (!PushedWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AsyncAction_PushSoftWidget] 没有创建出 Widget（检查 Stack Tag 是否正确、Widget 类是否继承 UWidget_ActivatableBase）"));
			SetReadyToDestroy();
			return;
		}

		if (InState == EAsyncPushWidgetState::OnCreatedBeforePush)
		{
			if (APlayerController* OwningPC = CachedOwingPC.Get())
			{
				PushedWidget->SetOwningPlayer(OwningPC);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[AsyncAction_PushSoftWidget] CachedOwingPC 无效：蓝图里 PlayerController 引脚可能没接"));
			}

			OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
		}
		else if (InState == EAsyncPushWidgetState::AfterPush)
		{
			OnWidgetCreatedAfterPush.Broadcast(PushedWidget);

			if (bCachedFocusOnNewlyPushedWidget)
			{
				if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
				{
					if (APlayerController* OwningPC = CachedOwingPC.Get())
					{
						WidgetToFocus->SetUserFocus(OwningPC);
					}
				}
			}

			SetReadyToDestroy();
		}
	});
}
