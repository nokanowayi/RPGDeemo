// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "FrontendCommonButtonBase.h"
#include "FrontendTabListWidgetBase.generated.h"

/**
 * 
 */
UCLASS(Abstract,BlueprintType,meta=(DisableNativeTick))
class RPGDEEMO_API UFrontendTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
public:
	void RequestRegisterTab(const FName& TabID,const FText& TabDisplayName);
	
private:
	
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	
	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Frontend Tab List Settings",meta=(AllowPrivateAccess="true",ClampMin="1",ClampMax="10"))
	int32 DebugEditorPreviewTabCount = 3;

	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Frontend Tab List Settings",meta=(AllowPrivateAccess="true"))
	TSubclassOf<UFrontendCommonButtonBase> TabButtonEntryWidgetClass;
};
