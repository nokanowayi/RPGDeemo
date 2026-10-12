// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widget_ListEntry_Base.generated.h"

/**
 * 
 */
UCLASS(Abstract,blueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UWidget_ListEntry_Base : public UCommonUserWidget,public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(BlueprintReadOnly,meta = (BindWidgetOptional,AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_SettingDisplayName;
};
