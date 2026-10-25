// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FrontendCommonButtonBase.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract,BlueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UFrontendCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InText);

	UFUNCTION(BlueprintCallable)
	FText GetButtonText() const;
	
private:
	virtual void NativePreConstruct() override;

	virtual void NativeOnCurrentTextStyleChanged() override;

	virtual void NativeOnHovered() override;

	virtual void NativeOnUnhovered() override;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UCommonTextBlock* CommonTextBlock_ButtonText;

	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="FrontendButton" ,meta=(AllowPrivateAccess = "true"))
	FText ButtonDisplayText;

	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="FrontendButton" ,meta=(AllowPrivateAccess = "true"))
	bool bUserUpperCaseForButtonText = false;

	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="FrontendButton" ,meta=(AllowPrivateAccess = "true"))
	FText ButtonDescriptionText;
};
