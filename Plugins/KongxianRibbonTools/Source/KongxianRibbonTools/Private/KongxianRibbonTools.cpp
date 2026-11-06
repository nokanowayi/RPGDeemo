#include "Modules/ModuleManager.h"
#include "HAL/IConsoleManager.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Editor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimMontage.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Serialization/ArchiveReplaceObjectRef.h"
#include "Engine/SkeletalMeshSocket.h"

DEFINE_LOG_CATEGORY_STATIC(LogKongxianRibbon, Log, All);

namespace KongxianRibbon
{
static const TCHAR* MeshPath = TEXT("/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady.SK_Kongxian_UE5_RetargetReady");
static const TCHAR* PackagePath = TEXT("/Game/Assets/Kongxian/PA_Kongxian_RibbonCollision");
static const FName AssetName(TEXT("PA_Kongxian_RibbonCollision"));

static bool Report(UPhysicsAsset* Asset, USkeletalMesh* Mesh, const FString& Operation)
{
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("operation"), Operation);
    Root->SetStringField(TEXT("asset"), Asset->GetPathName());
    Root->SetStringField(TEXT("mesh"), Mesh->GetPathName());
    TArray<TSharedPtr<FJsonValue>> Bodies, Constraints;
    bool bValid = true;
    int32 RibbonCount = 0;
    for (const USkeletalBodySetup* Body : Asset->SkeletalBodySetups)
    {
        TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        const int32 BoneIndex = Mesh->GetRefSkeleton().FindBoneIndex(Body->BoneName);
        Item->SetStringField(TEXT("bone"), Body->BoneName.ToString());
        Item->SetNumberField(TEXT("bone_index"), BoneIndex);
        Item->SetBoolField(TEXT("simulated"), Body->PhysicsType == PhysType_Simulated);
        bValid &= BoneIndex != INDEX_NONE && Body->AggGeom.SphylElems.Num() == 1;
        if (Body->BoneName.ToString().StartsWith(TEXT("handbelt"))) ++RibbonCount;
        if (!Body->AggGeom.SphylElems.IsEmpty())
        {
            const FKSphylElem& Capsule = Body->AggGeom.SphylElems[0];
            Item->SetNumberField(TEXT("radius_cm"), Capsule.Radius);
            Item->SetNumberField(TEXT("cylinder_length_cm"), Capsule.Length);
            Item->SetStringField(TEXT("center_bone_local"), Capsule.Center.ToString());
        }
        Bodies.Add(MakeShared<FJsonValueObject>(Item));
    }
    for (const UPhysicsConstraintTemplate* Template : Asset->ConstraintSetup)
    {
        const FConstraintInstance& C = Template->DefaultInstance;
        TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("child"), C.ConstraintBone1.ToString());
        Item->SetStringField(TEXT("parent"), C.ConstraintBone2.ToString());
        bValid &= Asset->FindBodyIndex(C.ConstraintBone1) != INDEX_NONE && Asset->FindBodyIndex(C.ConstraintBone2) != INDEX_NONE;
        Constraints.Add(MakeShared<FJsonValueObject>(Item));
    }
    bValid &= RibbonCount == 12 && Constraints.Num() == 11 && Bodies.Num() >= 28;
    Root->SetArrayField(TEXT("bodies"), Bodies);
    Root->SetArrayField(TEXT("constraints"), Constraints);
    Root->SetNumberField(TEXT("body_count"), Bodies.Num());
    Root->SetNumberField(TEXT("ribbon_body_count"), RibbonCount);
    Root->SetNumberField(TEXT("constraint_count"), Constraints.Num());
    Root->SetBoolField(TEXT("valid"), bValid);
    FString Json;
    FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Json));
    const FString Directory = FPaths::ProjectSavedDir() / TEXT("KongxianRibbon");
    IFileManager::Get().MakeDirectory(*Directory, true);
    FFileHelper::SaveStringToFile(Json, *(Directory / TEXT("physics_asset_report.json")));
    UE_LOG(LogKongxianRibbon, Display, TEXT("%s: valid=%d bodies=%d constraints=%d; report=%s"), *Operation, bValid, Bodies.Num(), Constraints.Num(), *Directory);
    return bValid;
}

static void Build()
{
    USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, MeshPath);
    if (!Mesh) { UE_LOG(LogKongxianRibbon, Error, TEXT("Reference skeletal mesh missing")); return; }
    FString Filename = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
    if (FPaths::FileExists(Filename) || FindPackage(nullptr, PackagePath))
    {
        UE_LOG(LogKongxianRibbon, Warning, TEXT("Destination exists; refusing overwrite. Run Kongxian.ValidateRibbonPhysics.")); return;
    }
    const FReferenceSkeleton& Ref = Mesh->GetRefSkeleton();
    TArray<FTransform> Global;
    for (int32 I=0; I<Ref.GetNum(); ++I)
    {
        const int32 Parent = Ref.GetParentIndex(I);
        Global.Add(Parent == INDEX_NONE ? Ref.GetRefBonePose()[I] : Ref.GetRefBonePose()[I] * Global[Parent]);
    }
    TArray<FName> Required;
    for (int32 I=0; I<12; ++I) Required.Add(FName(*FString::Printf(TEXT("handbelt%d"), I)));
    const TCHAR* BodyNames[] = {TEXT("pelvis"),TEXT("spine_01"),TEXT("spine_03"),TEXT("head"),TEXT("upperarm_l"),TEXT("lowerarm_l"),TEXT("hand_l"),TEXT("upperarm_r"),TEXT("lowerarm_r"),TEXT("hand_r"),TEXT("thigh_l"),TEXT("calf_l"),TEXT("thigh_r"),TEXT("calf_r"),TEXT("foot_l"),TEXT("foot_r")};
    for (const TCHAR* Name : BodyNames) Required.Add(FName(Name));
    for (FName Name : Required)
        if (Ref.FindBoneIndex(Name) == INDEX_NONE) { UE_LOG(LogKongxianRibbon, Error, TEXT("Required bone missing: %s"), *Name.ToString()); return; }
    auto Pose = [&](FName Name) -> const FTransform& { return Global[Ref.FindBoneIndex(Name)]; };
    auto Pos = [&](FName Name) { return Pose(Name).GetLocation(); };
    UPackage* Package = CreatePackage(PackagePath);
    UPhysicsAsset* Asset = NewObject<UPhysicsAsset>(Package, AssetName, RF_Public | RF_Standalone | RF_Transactional);
    // Eleven lightweight constrained links need more contact iterations than
    // the six-iteration default when wind pushes the ribbon against a limb.
    Asset->SolverSettings.PositionIterations = 16;
    Asset->SolverSettings.VelocityIterations = 4;
    Asset->SolverSettings.ProjectionIterations = 2;
    Asset->SolverSettings.FixedTimeStep = 1.f / 120.f;
    auto Capsule = [&](FName Bone, FVector Start, FVector End, float Radius, bool bSimulated)
    {
        USkeletalBodySetup* Body = NewObject<USkeletalBodySetup>(Asset, NAME_None, RF_Transactional);
        Body->BoneName = Bone;
        Body->PhysicsType = bSimulated ? PhysType_Simulated : PhysType_Kinematic;
        Body->DefaultInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Body->DefaultInstance.SetObjectType(ECC_PhysicsBody);
        Body->DefaultInstance.SetResponseToAllChannels(ECR_Block);
        Body->DefaultInstance.LinearDamping = 1.f;
        Body->DefaultInstance.AngularDamping = 3.f;
        Body->DefaultInstance.SetMassOverride(bSimulated ? .015f : 1.f, true);
        const FTransform& BonePose = Pose(Bone);
        const FVector A = BonePose.InverseTransformPosition(Start), B = BonePose.InverseTransformPosition(End);
        FKSphylElem Shape;
        Shape.Center = (A+B)*.5;
        Shape.Radius = Radius;
        Shape.Length = FMath::Max(0.f, float((B-A).Length()) - 2.f*Radius);
        Shape.Rotation = FQuat::FindBetweenNormals(FVector::UpVector, (B-A).GetSafeNormal(SMALL_NUMBER, FVector::UpVector)).Rotator();
        Body->AggGeom.SphylElems.Add(Shape);
        Asset->SkeletalBodySetups.Add(Body);
    };
    for (int32 I=0; I<12; ++I)
    {
        const FName Bone(*FString::Printf(TEXT("handbelt%d"), I));
        const FVector A = Pos(Bone);
        const FVector B = I<11 ? Pos(FName(*FString::Printf(TEXT("handbelt%d"), I+1))) : A+(A-Pos(FName(TEXT("handbelt10"))));
        Capsule(Bone, A, B, FMath::Clamp(float((B-A).Length())*.22f, 2.f, 3.5f), I>0);
    }
    struct FBodyFit { const TCHAR* Bone; const TCHAR* End; float Radius; };
    const FBodyFit Fits[] = {
        {TEXT("pelvis"),TEXT("spine_01"),11}, {TEXT("spine_01"),TEXT("spine_03"),12},
        {TEXT("spine_03"),TEXT("neck_01"),14}, {TEXT("head"),nullptr,10},
        {TEXT("upperarm_l"),TEXT("lowerarm_l"),5}, {TEXT("lowerarm_l"),TEXT("hand_l"),4}, {TEXT("hand_l"),nullptr,4},
        {TEXT("upperarm_r"),TEXT("lowerarm_r"),5}, {TEXT("lowerarm_r"),TEXT("hand_r"),4}, {TEXT("hand_r"),nullptr,4},
        {TEXT("thigh_l"),TEXT("calf_l"),7}, {TEXT("calf_l"),TEXT("foot_l"),5},
        {TEXT("thigh_r"),TEXT("calf_r"),7}, {TEXT("calf_r"),TEXT("foot_r"),5},
        {TEXT("foot_l"),nullptr,5}, {TEXT("foot_r"),nullptr,5}
    };
    for (const FBodyFit& Fit : Fits)
    {
        const FName Bone(Fit.Bone);
        const FVector A=Pos(Bone);
        const int32 EndIndex=Fit.End ? Ref.FindBoneIndex(FName(Fit.End)) : INDEX_NONE;
        const FVector B=EndIndex!=INDEX_NONE ? Global[EndIndex].GetLocation() : A+FVector(0,0,Fit.Radius);
        Capsule(Bone,A,B,Fit.Radius,false);
    }
    Asset->UpdateBodySetupIndexMap();
    for (int32 I=0; I<Asset->SkeletalBodySetups.Num(); ++I)
        for (int32 J=I+1; J<Asset->SkeletalBodySetups.Num(); ++J)
            if ((I<12 && J<12) || (I>=12 && J>=12)) Asset->DisableCollision(I,J);
    for (int32 I=1; I<12; ++I)
    {
        UPhysicsConstraintTemplate* Template=NewObject<UPhysicsConstraintTemplate>(Asset,NAME_None,RF_Transactional);
        FConstraintInstance& C=Template->DefaultInstance;
        C.JointName=FName(*FString::Printf(TEXT("RibbonJoint%d"),I));
        C.ConstraintBone1=FName(*FString::Printf(TEXT("handbelt%d"),I));
        C.ConstraintBone2=FName(*FString::Printf(TEXT("handbelt%d"),I-1));
        C.SetLinearXLimit(LCM_Locked,0); C.SetLinearYLimit(LCM_Locked,0); C.SetLinearZLimit(LCM_Locked,0);
        C.SetAngularSwing1Limit(ACM_Limited,35); C.SetAngularSwing2Limit(ACM_Limited,35); C.SetAngularTwistLimit(ACM_Limited,20);
        C.SetDisableCollision(true);
        C.SetProjectionParams(true,.1f,.1f,5.f,15.f);
        const FVector Anchor=Pos(C.ConstraintBone1);
        const FVector Axis=(Anchor-Pos(C.ConstraintBone2)).GetSafeNormal(SMALL_NUMBER, FVector::ForwardVector);
        const FTransform Joint(FQuat::FindBetweenNormals(FVector::ForwardVector,Axis),Anchor);
        C.SetRefFrame(EConstraintFrame::Frame1,Joint.GetRelativeTransform(Pose(C.ConstraintBone1)));
        C.SetRefFrame(EConstraintFrame::Frame2,Joint.GetRelativeTransform(Pose(C.ConstraintBone2)));
        Asset->ConstraintSetup.Add(Template);
    }
    Asset->SetPreviewMesh(Mesh);
    Asset->UpdateBoundsBodiesArray();
    Asset->MarkPackageDirty();
    if (!Report(Asset,Mesh,TEXT("build"))) { UE_LOG(LogKongxianRibbon,Error,TEXT("Validation failed; asset not saved")); return; }
    FAssetRegistryModule::AssetCreated(Asset);
    FSavePackageArgs Args; Args.TopLevelFlags=RF_Public|RF_Standalone; Args.SaveFlags=SAVE_NoError;
    const bool bSaved=UPackage::SavePackage(Package,Asset,*Filename,Args);
    UE_LOG(LogKongxianRibbon,Display,TEXT("Physics asset save success=%d: %s"),bSaved,*Filename);
}

static TArray<TSharedPtr<FJsonValue>> VectorJson(const FVector& V)
{
    return { MakeShared<FJsonValueNumber>(V.X), MakeShared<FJsonValueNumber>(V.Y), MakeShared<FJsonValueNumber>(V.Z) };
}

// Uses reflection deliberately: diagnostic-only tools need no AnimGraphRuntime link dependency.
static void DumpRibbonRuntime()
{
    UWorld* World = GEditor ? (GEditor->PlayWorld ? GEditor->PlayWorld.Get() : GEditor->GetEditorWorldContext().World()) : nullptr;
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("world"), GetPathNameSafe(World));
    Root->SetBoolField(TEXT("pie"), GEditor && GEditor->PlayWorld != nullptr);
    Root->SetStringField(TEXT("capsule_source"), TEXT("Current evaluated bone transforms, not direct Chaos particle transforms"));
    TArray<TSharedPtr<FJsonValue>> Components;
    for (TObjectIterator<USkeletalMeshComponent> It; It; ++It)
    {
        USkeletalMeshComponent* Component = *It;
        USkeletalMesh* Mesh = Component->GetSkeletalMeshAsset();
        if (!World || Component->GetWorld() != World || !Mesh || !Mesh->GetName().Contains(TEXT("SK_Kongxian_UE5_RetargetReady"))) continue;
        // Complete parallel animation evaluation before reading reflected node data/bone matrices.
        Component->HandleExistingParallelEvaluationTask(true, true);
        TSharedRef<FJsonObject> Entry = MakeShared<FJsonObject>();
        Entry->SetStringField(TEXT("component"), Component->GetPathName());
        Entry->SetStringField(TEXT("owner"), GetPathNameSafe(Component->GetOwner()));
        Entry->SetStringField(TEXT("component_transform"), Component->GetComponentTransform().ToString());
        UAnimInstance* Instance = Component->GetPostProcessInstance();
        Entry->SetStringField(TEXT("post_process_instance"), GetPathNameSafe(Instance));
        TArray<TSharedPtr<FJsonValue>> Nodes;
        UPhysicsAsset* PhysicsAsset = nullptr;
        if (Instance)
        {
            for (TFieldIterator<FStructProperty> Prop(Instance->GetClass()); Prop; ++Prop)
            {
                if (!Prop->Struct || Prop->Struct->GetFName() != FName(TEXT("AnimNode_RigidBody"))) continue;
                void* NodeData = Prop->ContainerPtrToValuePtr<void>(Instance);
                TSharedRef<FJsonObject> Node = MakeShared<FJsonObject>();
                Node->SetStringField(TEXT("property"), Prop->GetName());
                for (TFieldIterator<FProperty> Field(Prop->Struct); Field; ++Field)
                {
                    const FName Name = Field->GetFName();
                    if (Name == TEXT("ExternalForce") || Name == TEXT("OverridePhysicsAsset") || Name == TEXT("SimulationSpace") ||
                        Name == TEXT("bEnableWorldGeometry") || Name == TEXT("OverlapChannel") || Name == TEXT("CachedBoundsScale") ||
                        Name == TEXT("bOverrideWorldGravity") || Name == TEXT("OverrideWorldGravity") || Name == TEXT("Alpha"))
                    {
                        FString Value;
                        void* Address = Field->ContainerPtrToValuePtr<void>(NodeData);
                        Field->ExportTextItem_Direct(Value, Address, nullptr, Instance, PPF_None);
                        Node->SetStringField(Field->GetName(), Value);
                        if (Name == TEXT("OverridePhysicsAsset"))
                            if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(*Field))
                                PhysicsAsset = Cast<UPhysicsAsset>(ObjectProperty->GetObjectPropertyValue(Address));
                    }
                }
                Nodes.Add(MakeShared<FJsonValueObject>(Node));
            }
        }
        Entry->SetArrayField(TEXT("rigid_body_nodes"), Nodes);
        if (!PhysicsAsset) PhysicsAsset = Component->GetPhysicsAsset();
        Entry->SetStringField(TEXT("physics_asset"), GetPathNameSafe(PhysicsAsset));
        TArray<TSharedPtr<FJsonValue>> Bones, Capsules;
        for (int32 I=0; I<12; ++I)
        {
            const FName Bone(*FString::Printf(TEXT("handbelt%d"), I));
            TSharedRef<FJsonObject> BoneJson = MakeShared<FJsonObject>();
            BoneJson->SetStringField(TEXT("bone"), Bone.ToString());
            BoneJson->SetNumberField(TEXT("index"), Component->GetBoneIndex(Bone));
            BoneJson->SetArrayField(TEXT("world_position"), VectorJson(Component->GetBoneTransform(Bone).GetLocation()));
            Bones.Add(MakeShared<FJsonValueObject>(BoneJson));
        }
        if (PhysicsAsset)
            for (USkeletalBodySetup* Body : PhysicsAsset->SkeletalBodySetups)
            {
                if (!Body) continue;
                const FTransform BoneWorld = Component->GetBoneTransform(Body->BoneName);
                for (const FKSphylElem& Capsule : Body->AggGeom.SphylElems)
                {
                    const FTransform CapsuleWorld = FTransform(Capsule.Rotation, Capsule.Center) * BoneWorld;
                    const FVector HalfAxis = CapsuleWorld.TransformVector(FVector(0,0,Capsule.Length*.5f));
                    TSharedRef<FJsonObject> Shape = MakeShared<FJsonObject>();
                    Shape->SetStringField(TEXT("bone"), Body->BoneName.ToString());
                    Shape->SetBoolField(TEXT("simulated"), Body->PhysicsType == PhysType_Simulated);
                    Shape->SetArrayField(TEXT("world_center"), VectorJson(CapsuleWorld.GetLocation()));
                    Shape->SetArrayField(TEXT("world_endpoint_a"), VectorJson(CapsuleWorld.GetLocation()-HalfAxis));
                    Shape->SetArrayField(TEXT("world_endpoint_b"), VectorJson(CapsuleWorld.GetLocation()+HalfAxis));
                    Shape->SetNumberField(TEXT("radius_cm"), Capsule.Radius * CapsuleWorld.GetScale3D().GetAbsMax());
                    Shape->SetNumberField(TEXT("cylinder_length_cm"), Capsule.Length);
                    Capsules.Add(MakeShared<FJsonValueObject>(Shape));
                }
            }
        Entry->SetArrayField(TEXT("ribbon_bones"), Bones);
        Entry->SetArrayField(TEXT("capsules"), Capsules);
        Components.Add(MakeShared<FJsonValueObject>(Entry));
    }
    Root->SetArrayField(TEXT("components"), Components);
    Root->SetNumberField(TEXT("component_count"), Components.Num());
    FString Json;
    FJsonSerializer::Serialize(Root,TJsonWriterFactory<>::Create(&Json));
    const FString Directory = FPaths::ProjectSavedDir()/TEXT("KongxianRibbon");
    IFileManager::Get().MakeDirectory(*Directory,true);
    const FString Filename = Directory/TEXT("runtime_report.json");
    const bool bSaved = FFileHelper::SaveStringToFile(Json,*Filename);
    UE_LOG(LogKongxianRibbon,Display,TEXT("Runtime report: components=%d saved=%d file=%s"),Components.Num(),bSaved,*Filename);
}

static void SyncHeroSlots()
{
    if (!GEditor || GEditor->PlayWorld)
    {
        UE_LOG(LogKongxianRibbon, Error, TEXT("SyncHeroSlots requires Editor mode with PIE stopped."));
        return;
    }
    USkeleton* Source = LoadObject<USkeleton>(nullptr, TEXT("/Game/Assets/HeroCharacter/SK_Hero.SK_Hero"));
    USkeletalMesh* TargetMesh = LoadObject<USkeletalMesh>(nullptr, MeshPath);
    USkeleton* Target = TargetMesh ? TargetMesh->GetSkeleton() : nullptr;
    if (!Source || !Target || Source == Target)
    {
        UE_LOG(LogKongxianRibbon, Error, TEXT("SyncHeroSlots: source or separate target skeleton missing."));
        return;
    }
    Target->Modify();
    int32 Changed = 0;
    TArray<TSharedPtr<FJsonValue>> Slots;
    for (const FAnimSlotGroup& Group : Source->GetSlotGroups())
    {
        Target->AddSlotGroupName(Group.GroupName);
        for (const FName Slot : Group.SlotNames)
        {
            const bool bNeedsChange = !Target->ContainsSlotName(Slot) || Target->GetSlotGroupName(Slot) != Group.GroupName;
            if (bNeedsChange)
            {
                Target->SetSlotGroupName(Slot, Group.GroupName);
                ++Changed;
            }
            TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
            Item->SetStringField(TEXT("slot"), Slot.ToString());
            Item->SetStringField(TEXT("group"), Group.GroupName.ToString());
            Item->SetBoolField(TEXT("registered"), Target->ContainsSlotName(Slot) && Target->GetSlotGroupName(Slot) == Group.GroupName);
            Item->SetBoolField(TEXT("changed"), bNeedsChange);
            Slots.Add(MakeShared<FJsonValueObject>(Item));
        }
    }
    Target->MarkPackageDirty();
    UPackage* Package = Target->GetOutermost();
    const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    const bool bSaved = UPackage::SavePackage(Package, Target, *Filename, Args);
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("source_skeleton"), Source->GetPathName());
    Root->SetStringField(TEXT("target_skeleton"), Target->GetPathName());
    Root->SetNumberField(TEXT("slots_changed"), Changed);
    Root->SetBoolField(TEXT("target_saved"), bSaved);
    Root->SetBoolField(TEXT("source_modified"), false);
    Root->SetArrayField(TEXT("slots"), Slots);
    FString Json;
    FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Json));
    const FString Directory = FPaths::ProjectSavedDir() / TEXT("KongxianRibbon");
    IFileManager::Get().MakeDirectory(*Directory, true);
    FFileHelper::SaveStringToFile(Json, *(Directory / TEXT("hero_slots_report.json")));
    UE_LOG(LogKongxianRibbon, Display, TEXT("SyncHeroSlots: registered=%d changed=%d target_saved=%d"), Slots.Num(), Changed, bSaved);
}

static void DuplicateHeroMontages()
{
    if (!GEditor || GEditor->PlayWorld)
    {
        UE_LOG(LogKongxianRibbon, Error, TEXT("DuplicateHeroMontages requires Editor mode with PIE stopped."));
        return;
    }
    USkeleton* SourceSkeleton = LoadObject<USkeleton>(nullptr, TEXT("/Game/Assets/HeroCharacter/SK_Hero.SK_Hero"));
    USkeletalMesh* TargetMesh = LoadObject<USkeletalMesh>(nullptr, MeshPath);
    USkeleton* TargetSkeleton = TargetMesh ? TargetMesh->GetSkeleton() : nullptr;
    if (!SourceSkeleton || !TargetSkeleton || SourceSkeleton == TargetSkeleton) return;
    IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    TArray<FAssetData> SourceData;
    Registry.GetAssetsByPath(FName(TEXT("/Game/PlayerCharacter/Montages")), SourceData, true);
    TArray<UAnimMontage*> Sources;
    TMap<UObject*, UObject*> Replacements;
    Replacements.Add(SourceSkeleton, TargetSkeleton);
    for (const FAssetData& Data : SourceData)
        if (UAnimMontage* Montage = Cast<UAnimMontage>(Data.GetAsset()))
            if (Montage->GetSkeleton() == SourceSkeleton) Sources.Add(Montage);
    if (Sources.IsEmpty())
    {
        UE_LOG(LogKongxianRibbon, Error, TEXT("No Hero gameplay montages found."));
        return;
    }
    // Preflight all referenced animation assets before creating any files.
    // Resolve to already-retargeted KX_ assets; do not retarget sequences again.
    TArray<TSharedPtr<FJsonValue>> Missing;
    for (UAnimMontage* Montage : Sources)
    {
        TArray<UAnimationAsset*> References;
        Montage->HandleAnimReferenceCollection(References, true);
        for (UAnimationAsset* Reference : References)
        {
            if (!Reference || Reference == Montage || Sources.Contains(Cast<UAnimMontage>(Reference)) || Replacements.Contains(Reference)) continue;
            const FString NewName = TEXT("KX_") + Reference->GetName();
            const FString NewPath = TEXT("/Game/Assets/Kongxian/HeroAnimation/") + NewName + TEXT(".") + NewName;
            UAnimationAsset* Retargeted = LoadObject<UAnimationAsset>(nullptr, *NewPath);
            if (Retargeted && Retargeted->GetSkeleton() == TargetSkeleton)
                Replacements.Add(Reference, Retargeted);
            else
                Missing.Add(MakeShared<FJsonValueString>(Reference->GetPathName()));
        }
    }
    const FString Directory = FPaths::ProjectSavedDir() / TEXT("KongxianRibbon");
    IFileManager::Get().MakeDirectory(*Directory, true);
    TSharedRef<FJsonObject> ReportJson = MakeShared<FJsonObject>();
    ReportJson->SetArrayField(TEXT("missing_retargeted_references"), Missing);
    ReportJson->SetBoolField(TEXT("source_modified"), false);
    TArray<TSharedPtr<FJsonValue>> Results;
    if (Missing.IsEmpty())
    {
        TMap<UAnimMontage*, UAnimMontage*> Copies;
        TSet<UAnimMontage*> Created;
        for (UAnimMontage* Montage : Sources)
        {
            FString RelativeFolder = FPackageName::GetLongPackagePath(Montage->GetOutermost()->GetName());
            RelativeFolder.RemoveFromStart(TEXT("/Game/PlayerCharacter/Montages"));
            const FString NewName = TEXT("KX_") + Montage->GetName();
            const FString NewPackageName = TEXT("/Game/Assets/Kongxian/HeroAnimation/Montages") + RelativeFolder + TEXT("/") + NewName;
            UAnimMontage* Copy = LoadObject<UAnimMontage>(nullptr, *(NewPackageName + TEXT(".") + NewName), nullptr, LOAD_NoWarn);
            if (!Copy)
            {
                UPackage* Package = CreatePackage(*NewPackageName);
                Copy = DuplicateObject<UAnimMontage>(Montage, Package, FName(*NewName));
                Copy->SetFlags(RF_Public | RF_Standalone | RF_Transactional);
                Copy->SetSkeleton(TargetSkeleton);
                FAssetRegistryModule::AssetCreated(Copy);
                Created.Add(Copy);
            }
            Copies.Add(Montage, Copy);
            Replacements.Add(Montage, Copy);
        }
        for (const TPair<UAnimMontage*, UAnimMontage*>& Pair : Copies)
        {
            UAnimMontage* Copy = Pair.Value;
            if (Created.Contains(Copy))
            {
                FArchiveReplaceObjectRef<UObject> Replace(Copy, Replacements,
                    EArchiveReplaceObjectFlags::IgnoreOuterRef | EArchiveReplaceObjectFlags::IgnoreArchetypeRef);
                Copy->PostEditChange();
                Copy->MarkPackageDirty();
            }
            bool bValid = Copy->GetSkeleton() == TargetSkeleton;
            TArray<UAnimationAsset*> References;
            Copy->HandleAnimReferenceCollection(References, true);
            for (UAnimationAsset* Reference : References)
                bValid &= !Reference || Reference->GetSkeleton() == TargetSkeleton;
            bool bSaved = !Created.Contains(Copy);
            if (bValid && Created.Contains(Copy))
            {
                UPackage* Package = Copy->GetOutermost();
                const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
                IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
                FSavePackageArgs Args;
                Args.TopLevelFlags = RF_Public | RF_Standalone;
                Args.SaveFlags = SAVE_NoError;
                bSaved = UPackage::SavePackage(Package, Copy, *Filename, Args);
            }
            TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
            Item->SetStringField(TEXT("source"), Pair.Key->GetPathName());
            Item->SetStringField(TEXT("target"), Copy->GetPathName());
            Item->SetBoolField(TEXT("created"), Created.Contains(Copy));
            Item->SetBoolField(TEXT("all_animation_references_use_target_skeleton"), bValid);
            Item->SetBoolField(TEXT("saved"), bSaved);
            Results.Add(MakeShared<FJsonValueObject>(Item));
        }
    }
    ReportJson->SetArrayField(TEXT("montages"), Results);
    ReportJson->SetNumberField(TEXT("source_montage_count"), Sources.Num());
    FString Json;
    FJsonSerializer::Serialize(ReportJson, TJsonWriterFactory<>::Create(&Json));
    FFileHelper::SaveStringToFile(Json, *(Directory / TEXT("hero_gameplay_montages_report.json")));
    UE_LOG(LogKongxianRibbon, Display, TEXT("DuplicateHeroMontages: sources=%d results=%d missing=%d. No source assets modified."), Sources.Num(), Results.Num(), Missing.Num());
}

static void SyncHeroSockets()
{
    if (GEditor && GEditor->PlayWorld) return;
    USkeletalMesh* Source=LoadObject<USkeletalMesh>(nullptr,TEXT("/Game/Assets/HeroCharacter/SK_CharM_Barbarous.SK_CharM_Barbarous"));
    USkeletalMesh* Target=LoadObject<USkeletalMesh>(nullptr,MeshPath);
    if (!Source || !Target) return;
    USkeleton* Dest=Target->GetSkeleton();
    auto GlobalPose=[](const FReferenceSkeleton& Ref)
    {
        TArray<FTransform> Out;
        for(int32 I=0;I<Ref.GetNum();++I)
            Out.Add(Ref.GetParentIndex(I)<0?Ref.GetRefBonePose()[I]:Ref.GetRefBonePose()[I]*Out[Ref.GetParentIndex(I)]);
        return Out;
    };
    const auto S=GlobalPose(Source->GetRefSkeleton()), T=GlobalPose(Target->GetRefSkeleton());
    TArray<TSharedPtr<FJsonValue>> Entries;
    for (const USkeletalMeshSocket* Socket:Source->GetSkeleton()->Sockets)
    {
        const int32 SI=Source->GetRefSkeleton().FindBoneIndex(Socket->BoneName), TI=Target->GetRefSkeleton().FindBoneIndex(Socket->BoneName);
        if(SI==INDEX_NONE || TI==INDEX_NONE) continue;
        bool Exists=false;for(const auto& Existing:Dest->Sockets) Exists|=Existing->SocketName==Socket->SocketName;
        if(Exists) continue;
        USkeletalMeshSocket* Copy=DuplicateObject<USkeletalMeshSocket>(Socket,Dest);
        // Preserve the Hero attachment orientation in reference component space,
        // then express it in the different MMD bone frame. Scale offset to fit.
        const FTransform World=Socket->GetSocketLocalTransform()*S[SI];
        FTransform Desired=World;
        Desired.SetLocation(T[TI].GetLocation()+(World.GetLocation()-S[SI].GetLocation())*.8);
        Copy->SetSocketLocalTransform(Desired.GetRelativeTransform(T[TI]));
        Dest->Sockets.Add(Copy);
        TSharedRef<FJsonObject> Item=MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("socket"),Copy->SocketName.ToString());
        Item->SetStringField(TEXT("bone"),Copy->BoneName.ToString());
        Item->SetStringField(TEXT("local_transform"),Copy->GetSocketLocalTransform().ToString());
        Entries.Add(MakeShared<FJsonValueObject>(Item));
    }
    Dest->MarkPackageDirty();
    FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;
    const bool Saved=UPackage::SavePackage(Dest->GetOutermost(),Dest,*FPackageName::LongPackageNameToFilename(Dest->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension()),Args);
    TSharedRef<FJsonObject> R=MakeShared<FJsonObject>();R->SetArrayField(TEXT("added"),Entries);R->SetBoolField(TEXT("saved"),Saved);
    FString Json;FJsonSerializer::Serialize(R,TJsonWriterFactory<>::Create(&Json));
    FFileHelper::SaveStringToFile(Json,*(FPaths::ProjectSavedDir()/TEXT("KongxianRibbon/hero_sockets_report.json")));
}

static void Validate()
{
    UPhysicsAsset* Asset=LoadObject<UPhysicsAsset>(nullptr,*(FString(PackagePath)+TEXT(".")+AssetName.ToString()));
    USkeletalMesh* Mesh=LoadObject<USkeletalMesh>(nullptr,MeshPath);
    if (Asset && Mesh) Report(Asset,Mesh,TEXT("validate"));
    else UE_LOG(LogKongxianRibbon,Error,TEXT("Physics asset or reference mesh missing"));
}
}

class FKongxianRibbonToolsModule final : public IModuleInterface
{
    IConsoleObject* BuildCommand=nullptr;
    IConsoleObject* ValidateCommand=nullptr;
    IConsoleObject* DumpCommand=nullptr;
    IConsoleObject* SyncSlotsCommand=nullptr;
    IConsoleObject* DuplicateMontagesCommand=nullptr;
    IConsoleObject* SocketsCommand=nullptr;
public:
    virtual void StartupModule() override
    {
        SocketsCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Kongxian.SyncHeroSockets"),TEXT("Add adapted Hero sockets without overwriting existing sockets."),FConsoleCommandDelegate::CreateStatic(&KongxianRibbon::SyncHeroSockets));
        BuildCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Kongxian.BuildRibbonPhysics"),TEXT("Create new ribbon and character collision physics asset; refuses overwrite."),FConsoleCommandDelegate::CreateStatic(&KongxianRibbon::Build));
        ValidateCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Kongxian.ValidateRibbonPhysics"),TEXT("Validate ribbon collision asset and write JSON report."),FConsoleCommandDelegate::CreateStatic(&KongxianRibbon::Validate));
        DumpCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Kongxian.DumpRibbonRuntime"),TEXT("Dump evaluated ribbon bone/capsule transforms and reflected RigidBody settings to runtime_report.json."),FConsoleCommandDelegate::CreateStatic(&KongxianRibbon::DumpRibbonRuntime));
        SyncSlotsCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Kongxian.SyncHeroSlots"),TEXT("Copy Hero animation slot groups to Kongxian skeleton, preserve unrelated slots, save target and write hero_slots_report.json. PIE must be stopped."),FConsoleCommandDelegate::CreateStatic(&KongxianRibbon::SyncHeroSlots));
        DuplicateMontagesCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Kongxian.DuplicateHeroMontages"),TEXT("Copy Hero gameplay montages and remap animation references to existing KX_ assets. Does not retarget sequences or overwrite existing montages."),FConsoleCommandDelegate::CreateStatic(&KongxianRibbon::DuplicateHeroMontages));
    }
    virtual void ShutdownModule() override
    {
        if (SocketsCommand) IConsoleManager::Get().UnregisterConsoleObject(SocketsCommand);
        if (BuildCommand) IConsoleManager::Get().UnregisterConsoleObject(BuildCommand);
        if (ValidateCommand) IConsoleManager::Get().UnregisterConsoleObject(ValidateCommand);
        if (DumpCommand) IConsoleManager::Get().UnregisterConsoleObject(DumpCommand);
        if (SyncSlotsCommand) IConsoleManager::Get().UnregisterConsoleObject(SyncSlotsCommand);
        if (DuplicateMontagesCommand) IConsoleManager::Get().UnregisterConsoleObject(DuplicateMontagesCommand);
    }
};
IMPLEMENT_MODULE(FKongxianRibbonToolsModule,KongxianRibbonTools)
