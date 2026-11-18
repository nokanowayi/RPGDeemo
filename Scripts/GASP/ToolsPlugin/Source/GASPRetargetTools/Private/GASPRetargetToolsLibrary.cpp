#include "GASPRetargetToolsLibrary.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/BlendProfile.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Animation/Skeleton.h"
#include "Curves/CurveFloat.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"

DEFINE_LOG_CATEGORY_STATIC(LogGASPRetargetTools, Log, All);

namespace GASPRetargetTools
{
    static constexpr const TCHAR* OutputPackagePrefix = TEXT("/Game/Assets/Kongxian/HeroAnimation/GASP/");

    static bool IsBlendOutName(const FString& Name)
    {
        return Name.Contains(TEXT("TraversalBlendOut")) || Name.Contains(TEXT("MontageBlendOut"));
    }

    static bool IsBlendOutEvent(const FAnimNotifyEvent& Event)
    {
        return IsBlendOutName(Event.NotifyName.ToString())
            || (IsValid(Event.NotifyStateClass) && IsBlendOutName(Event.NotifyStateClass->GetClass()->GetName()));
    }

    static FString WriteJson(const TSharedRef<FJsonObject>& Object)
    {
        FString Result;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Result);
        FJsonSerializer::Serialize(Object, Writer);
        return Result;
    }

    static FString ErrorJson(const TCHAR* Message)
    {
        const TSharedRef<FJsonObject> Result = MakeShared<FJsonObject>();
        Result->SetStringField(TEXT("error"), Message);
        return WriteJson(Result);
    }

    // Use reflection to inspect plugin-defined notify settings without linking those plugins.
    // References are exported as text; only the inline root-motion modifier is expanded.
    static TSharedRef<FJsonObject> DescribeObject(const UObject* Object, int32 Depth = 0)
    {
        const TSharedRef<FJsonObject> Result = MakeShared<FJsonObject>();
        if (!IsValid(Object))
        {
            Result->SetBoolField(TEXT("is_null"), true);
            return Result;
        }

        Result->SetStringField(TEXT("class"), Object->GetClass()->GetPathName());
        Result->SetStringField(TEXT("object"), Object->GetPathName());
        const TSharedRef<FJsonObject> Properties = MakeShared<FJsonObject>();
        for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
        {
            const FProperty* Property = *It;
            if (Property->HasAnyPropertyFlags(CPF_Transient | CPF_Deprecated))
            {
                continue;
            }

            FString ExportedValue;
            Property->ExportTextItem_Direct(
                ExportedValue, Property->ContainerPtrToValuePtr<void>(Object), nullptr,
                const_cast<UObject*>(Object), PPF_None);
            Properties->SetStringField(Property->GetName(), ExportedValue);

            if (Depth < 1 && Property->GetFName() == TEXT("RootMotionModifier"))
            {
                if (const FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
                {
                    Result->SetObjectField(TEXT("root_motion_modifier"),
                        DescribeObject(ObjectProperty->GetObjectPropertyValue_InContainer(Object), Depth + 1));
                }
            }
        }
        Result->SetObjectField(TEXT("properties"), Properties);
        return Result;
    }

    static TSharedRef<FJsonObject> DescribeBlend(const FAlphaBlend& Blend)
    {
        const TSharedRef<FJsonObject> Result = MakeShared<FJsonObject>();
        Result->SetNumberField(TEXT("time"), Blend.GetBlendTime());
        Result->SetStringField(TEXT("option"), StaticEnum<EAlphaBlendOption>()->GetNameStringByValue(
            static_cast<int64>(Blend.GetBlendOption())));
        Result->SetStringField(TEXT("custom_curve"), GetPathNameSafe(Blend.GetCustomCurve()));
        return Result;
    }
}

bool UGASPRetargetToolsLibrary::SetMontageSlot(UAnimMontage* Montage, FName SlotName)
{
    if (!IsInGameThread() || !IsValid(Montage) || SlotName.IsNone())
    {
        UE_LOG(LogGASPRetargetTools, Warning, TEXT("SetMontageSlot requires a valid montage, slot and game thread."));
        return false;
    }

    if (!Montage->GetOutermost()->GetName().StartsWith(
        GASPRetargetTools::OutputPackagePrefix, ESearchCase::CaseSensitive))
    {
        UE_LOG(LogGASPRetargetTools, Warning, TEXT("Refusing to modify montage outside GASP output: %s"),
            *Montage->GetPathName());
        return false;
    }

    const USkeleton* Skeleton = Montage->GetSkeleton();
    if (!IsValid(Skeleton) || !Skeleton->ContainsSlotName(SlotName) || Montage->SlotAnimTracks.IsEmpty())
    {
        UE_LOG(LogGASPRetargetTools, Warning, TEXT("Montage %s has no tracks or its skeleton has no slot %s."),
            *Montage->GetPathName(), *SlotName.ToString());
        return false;
    }

    const bool bNeedsChange = Montage->SlotAnimTracks.ContainsByPredicate(
        [SlotName](const FSlotAnimationTrack& Track) { return Track.SlotName != SlotName; });
    if (bNeedsChange)
    {
        Montage->Modify();
        for (FSlotAnimationTrack& Track : Montage->SlotAnimTracks)
        {
            Track.SlotName = SlotName;
        }
        Montage->PostEditChange();
        Montage->MarkPackageDirty();
    }
    return true;
}

bool UGASPRetargetToolsLibrary::RestoreBlendOutNotify(UAnimMontage* Source, UAnimMontage* Target)
{
    using namespace GASPRetargetTools;
    if (!IsInGameThread() || !IsValid(Source) || !IsValid(Target) || Source == Target
        || !Target->GetOutermost()->GetName().StartsWith(OutputPackagePrefix, ESearchCase::CaseSensitive)
        || !FMath::IsNearlyEqual(Source->GetPlayLength(), Target->GetPlayLength(), 0.0001f))
    {
        UE_LOG(LogGASPRetargetTools, Warning, TEXT("RestoreBlendOutNotify requires distinct valid montages of equal length and a target in GASP output."));
        return false;
    }

    // Validate the entire source before touching the target. A source with the same
    // missing generated class must never erase the target's surviving events.
    TArray<const FAnimNotifyEvent*> SourceEvents;
    for (const FAnimNotifyEvent& Event : Source->Notifies)
    {
        if (!IsBlendOutEvent(Event))
        {
            continue;
        }
        if (!IsValid(Event.NotifyStateClass)
            || !IsBlendOutName(Event.NotifyStateClass->GetClass()->GetName())
            || !Target->SlotAnimTracks.IsValidIndex(Event.GetSlotIndex())
            || !Target->SlotAnimTracks.IsValidIndex(Event.EndLink.GetSlotIndex())
            || !Target->AnimNotifyTracks.IsValidIndex(Event.TrackIndex))
        {
            UE_LOG(LogGASPRetargetTools, Warning, TEXT("Cannot restore invalid BlendOut state/track from %s (%s)."),
                *Source->GetPathName(), *Event.NotifyName.ToString());
            return false;
        }
        SourceEvents.Add(&Event);
    }
    if (SourceEvents.IsEmpty())
    {
        UE_LOG(LogGASPRetargetTools, Warning, TEXT("No valid source BlendOut states found in %s."), *Source->GetPathName());
        return false;
    }

    // Duplicate before modifying the event array so the operation cannot partially
    // erase the target when source loading or inline-object creation fails.
    TArray<FAnimNotifyEvent> RestoredEvents;
    for (const FAnimNotifyEvent* SourceEvent : SourceEvents)
    {
        FAnimNotifyEvent Restored = *SourceEvent;
        UAnimNotifyState* SourceState = SourceEvent->NotifyStateClass.Get();
        Restored.NotifyStateClass = DuplicateObject<UAnimNotifyState>(SourceState, Target,
            MakeUniqueObjectName(Target, SourceState->GetClass(), SourceState->GetFName()));
        if (!IsValid(Restored.NotifyStateClass))
        {
            return false;
        }
        if (IsValid(SourceEvent->Notify))
        {
            UAnimNotify* SourceNotify = SourceEvent->Notify.Get();
            Restored.Notify = DuplicateObject<UAnimNotify>(SourceNotify, Target,
                MakeUniqueObjectName(Target, SourceNotify->GetClass(), SourceNotify->GetFName()));
            if (!IsValid(Restored.Notify))
            {
                return false;
            }
        }

        Restored.Link(Target, SourceEvent->GetTime(), SourceEvent->GetSlotIndex());
        Restored.EndLink.Link(Target, SourceEvent->EndLink.GetTime(), SourceEvent->EndLink.GetSlotIndex());
        RestoredEvents.Add(MoveTemp(Restored));
    }

    Target->Modify();
    Target->Notifies.RemoveAll([](const FAnimNotifyEvent& Event) { return IsBlendOutEvent(Event); });
    Target->Notifies.Append(MoveTemp(RestoredEvents));
    Target->SortNotifies();
    Target->RefreshCacheData();
    Target->PostEditChange();
    Target->MarkPackageDirty();
    return true;
}

FString UGASPRetargetToolsLibrary::DescribeMontage(UAnimMontage* Montage)
{
    using namespace GASPRetargetTools;
    if (!IsInGameThread() || !IsValid(Montage))
    {
        return ErrorJson(TEXT("A valid montage on the game thread is required."));
    }

    const TSharedRef<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("asset"), Montage->GetPathName());
    Result->SetStringField(TEXT("skeleton"), GetPathNameSafe(Montage->GetSkeleton()));
    Result->SetNumberField(TEXT("length"), Montage->GetPlayLength());
    Result->SetNumberField(TEXT("rate_scale"), Montage->RateScale);

    TArray<TSharedPtr<FJsonValue>> Slots;
    for (const FSlotAnimationTrack& Track : Montage->SlotAnimTracks)
    {
        const TSharedRef<FJsonObject> Slot = MakeShared<FJsonObject>();
        Slot->SetStringField(TEXT("name"), Track.SlotName.ToString());
        TArray<TSharedPtr<FJsonValue>> Segments;
        for (const FAnimSegment& Segment : Track.AnimTrack.AnimSegments)
        {
            const TSharedRef<FJsonObject> Data = MakeShared<FJsonObject>();
            Data->SetStringField(TEXT("animation"), GetPathNameSafe(Segment.GetAnimReference().Get()));
            Data->SetNumberField(TEXT("start_pos"), Segment.StartPos);
            Data->SetNumberField(TEXT("anim_start_time"), Segment.AnimStartTime);
            Data->SetNumberField(TEXT("anim_end_time"), Segment.AnimEndTime);
            Data->SetNumberField(TEXT("play_rate"), Segment.AnimPlayRate);
            Data->SetNumberField(TEXT("loop_count"), Segment.LoopingCount);
            Segments.Add(MakeShared<FJsonValueObject>(Data));
        }
        Slot->SetArrayField(TEXT("segments"), Segments);
        Slots.Add(MakeShared<FJsonValueObject>(Slot));
    }
    Result->SetArrayField(TEXT("slots"), Slots);

    TArray<TSharedPtr<FJsonValue>> Sections;
    for (const FCompositeSection& Section : Montage->CompositeSections)
    {
        const TSharedRef<FJsonObject> Data = MakeShared<FJsonObject>();
        Data->SetStringField(TEXT("name"), Section.SectionName.ToString());
        Data->SetNumberField(TEXT("start_time"), Section.GetTime());
        Data->SetStringField(TEXT("next_section"), Section.NextSectionName.ToString());
        Sections.Add(MakeShared<FJsonValueObject>(Data));
    }
    Result->SetArrayField(TEXT("sections"), Sections);
    Result->SetObjectField(TEXT("blend_in"), DescribeBlend(Montage->BlendIn));
    Result->SetObjectField(TEXT("blend_out"), DescribeBlend(Montage->BlendOut));
    Result->SetNumberField(TEXT("blend_out_trigger_time"), Montage->BlendOutTriggerTime);
    Result->SetBoolField(TEXT("enable_auto_blend_out"), Montage->bEnableAutoBlendOut);
    Result->SetStringField(TEXT("blend_mode_in"), StaticEnum<EMontageBlendMode>()->GetNameStringByValue(
        static_cast<int64>(Montage->BlendModeIn)));
    Result->SetStringField(TEXT("blend_mode_out"), StaticEnum<EMontageBlendMode>()->GetNameStringByValue(
        static_cast<int64>(Montage->BlendModeOut)));
    Result->SetStringField(TEXT("blend_profile_in"), GetPathNameSafe(Montage->BlendProfileIn.Get()));
    Result->SetStringField(TEXT("blend_profile_out"), GetPathNameSafe(Montage->BlendProfileOut.Get()));
    return WriteJson(Result);
}

FString UGASPRetargetToolsLibrary::DescribeNotifies(UAnimSequenceBase* Animation)
{
    using namespace GASPRetargetTools;
    if (!IsInGameThread() || !IsValid(Animation))
    {
        return ErrorJson(TEXT("A valid animation on the game thread is required."));
    }

    const TSharedRef<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("asset"), Animation->GetPathName());
    TArray<TSharedPtr<FJsonValue>> Notifies;
    for (const FAnimNotifyEvent& Event : Animation->Notifies)
    {
        const TSharedRef<FJsonObject> Data = MakeShared<FJsonObject>();
        Data->SetStringField(TEXT("name"), Event.NotifyName.ToString());
        Data->SetNumberField(TEXT("time"), Event.GetTime());
        Data->SetNumberField(TEXT("trigger_time"), Event.GetTriggerTime());
        Data->SetNumberField(TEXT("duration"), Event.GetDuration());
        Data->SetNumberField(TEXT("track_index"), Event.TrackIndex);
        Data->SetObjectField(TEXT("notify"), DescribeObject(Event.Notify.Get()));
        Data->SetObjectField(TEXT("notify_state"), DescribeObject(Event.NotifyStateClass.Get()));
        Notifies.Add(MakeShared<FJsonValueObject>(Data));
    }
    Result->SetArrayField(TEXT("notifies"), Notifies);
    return WriteJson(Result);
}
