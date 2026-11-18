#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GASPRetargetToolsLibrary.generated.h"

class UAnimMontage;
class UAnimSequenceBase;

/** Narrow, editor-only asset tools. Mutation never saves an asset automatically. */
UCLASS()
class GASPRETARGETTOOLS_API UGASPRetargetToolsLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Changes existing tracks only, and only for montages in the dedicated GASP output folder. */
    UFUNCTION(BlueprintCallable, Category = "GASP|Retarget")
    static bool SetMontageSlot(UAnimMontage* Montage, FName SlotName);

    /** Restores source BlendOut states as independent target inline objects; does not save. */
    UFUNCTION(BlueprintCallable, Category = "GASP|Retarget")
    static bool RestoreBlendOutNotify(UAnimMontage* Source, UAnimMontage* Target);

    /** Read-only JSON snapshot of montage tracks, segments, sections and blend settings. */
    UFUNCTION(BlueprintCallable, Category = "GASP|Retarget")
    static FString DescribeMontage(UAnimMontage* Montage);

    /** Read-only JSON, including reflected notify settings and nested RootMotionModifier settings. */
    UFUNCTION(BlueprintCallable, Category = "GASP|Retarget")
    static FString DescribeNotifies(UAnimSequenceBase* Animation);
};
