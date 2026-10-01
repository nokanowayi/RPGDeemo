// MyRPGDemo


#include "Subsystems/FrontendUISubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/AssetManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widget/Widget_PrimaryLayout.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "Widget/Widget_ConfirmScreen.h"

UFrontendUISubsystem* UFrontendUISubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				return GameInstance->GetSubsystem<UFrontendUISubsystem>();
			}
		}
	}

	return nullptr;
}

bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 只在「非专用服务器」上创建：客户端/单机需要前端 UI，专用服务器不需要
	if (const UGameInstance* GameInstance = Cast<UGameInstance>(Outer))
	{
		if (!GameInstance->IsDedicatedServerInstance())
		{
			TArray<UClass*> DerivedClasses;
			GetDerivedClasses(GetClass(), DerivedClasses);

			return DerivedClasses.IsEmpty();
		}
	}

	return false;
}

void UFrontendUISubsystem::RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InPrimaryLayoutWidget)
{
	if (!InPrimaryLayoutWidget)
	{
		return;
	}

	CreatedPrimaryLayoutWidget = InPrimaryLayoutWidget;

	Debug::Print(TEXT("Registered Primary Layout Widget"));
}

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InStackTag,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallBack)
{
	check(!InSoftWidgetClass.IsNull());

	// 空 tag（None）是最常见的调用方失误：蓝图里那个 GameplayTag 引脚忘了设值。
	// 提前挡掉并说清楚原因，比在 FindWidgetStackByTag 里崩掉强得多
	if (!InStackTag.IsValid())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[FrontendUISubsystem] PushSoftWidgetToStackAsync: InStackTag 是空的！检查蓝图里 'Push Soft Widget To Widget Stack' 节点的 Stack Tag 引脚（例如 UI.WidgetStack.Frontend）"));
		return;
	}

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([this, InStackTag, InSoftWidgetClass, AsyncPushStateCallBack]()
		{
			UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
			if (!LoadedWidgetClass)
			{
				return;
			}
			
			if (!CreatedPrimaryLayoutWidget)
			{
				return;
			}

			UCommonActivatableWidgetContainerBase* FoundStack = CreatedPrimaryLayoutWidget->FindWidgetStackByTag(InStackTag);
			if (!FoundStack)
			{
				return;
			}
			
			UWidget_ActivatableBase* CreatedWidget = FoundStack->AddWidget<UWidget_ActivatableBase>(
				LoadedWidgetClass,
				[AsyncPushStateCallBack](UWidget_ActivatableBase& NewWidget)
				{
					AsyncPushStateCallBack(EAsyncPushWidgetState::OnCreatedBeforePush, &NewWidget);
				});

			AsyncPushStateCallBack(EAsyncPushWidgetState::AfterPush, CreatedWidget);
		}));
}

void UFrontendUISubsystem::PushConfirmScreenToModalStackAsync(EConfirmScreenType InConfirmScreenType,
	const FText& InScreenTitle, const FText& InScreenMessage,
	TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{
	UConfirmScreenInfoObject* ConfirmScreenInfoObject = nullptr;
	switch (InConfirmScreenType)
	{
	case EConfirmScreenType::OK:
		ConfirmScreenInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMessage);
		break;
	case EConfirmScreenType::YesNo:
		ConfirmScreenInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMessage);
		break;
	case EConfirmScreenType::OKCancel:
		ConfirmScreenInfoObject = UConfirmScreenInfoObject::CreateOKCancelScreen(InScreenTitle, InScreenMessage);
		break;
	case EConfirmScreenType::Unknown:
		break;
	default: ;
	}
	check(ConfirmScreenInfoObject);

	PushSoftWidgetToStackAsync(WarriorGameplayTags::UI_WidgetStack_Modal,
		UWarriorFunctionLibrary::GetFrontendWidgetClassByTag(WarriorGameplayTags::UI_Widget_ConfirmScreen),
		[ConfirmScreenInfoObject,ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* InCreatedWidget)
		{
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				if (UWidget_ConfirmScreen* ConfirmScreen = Cast<UWidget_ConfirmScreen>(InCreatedWidget))
				{
					ConfirmScreen->InitConfirmScreen(ConfirmScreenInfoObject, ButtonClickedCallback);
				}
			}
		});
}
