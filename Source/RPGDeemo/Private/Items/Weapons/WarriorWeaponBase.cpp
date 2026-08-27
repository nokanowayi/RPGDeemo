// MyRPGDemo


#include "Items/Weapons/WarriorWeaponBase.h"

#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWarriorWeaponBase::AWarriorWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(WeaponMesh);
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnWeaponOverlapBegin);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this,&ThisClass::OnCollisionBoxEndOverlap);
}

void AWarriorWeaponBase::OnWeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponInstigator = nullptr;

	// 武器挂在谁身上，谁就是攻击者（优先于 Instigator：Instigator 可能被错误设置成玩家）
	if (AActor* AttachParent = GetAttachParentActor())
	{
		WeaponInstigator = Cast<APawn>(AttachParent);
	}
	if (!WeaponInstigator)
	{
		WeaponInstigator = Cast<APawn>(GetOwner());
	}
	if (!WeaponInstigator)
	{
		WeaponInstigator = GetInstigator<APawn>();
	}

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// 防止打到攻击者自己（自身 / Owner / Attach 的 Pawn）
		if (HitPawn == WeaponInstigator)
		{
			return;
		}

		if (WeaponInstigator && !UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponInstigator, HitPawn))
		{
			Debug::Print(FString::Printf(TEXT("Weapon Hit -> %s (by %s)"), *HitPawn->GetName(), *WeaponInstigator->GetName()), FColor::Green, 2.f);
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponInstigator = nullptr;

	// 与 BeginOverlap 一致：Attach 父级 > Owner > Instigator
	if (AActor* AttachParent = GetAttachParentActor())
	{
		WeaponInstigator = Cast<APawn>(AttachParent);
	}
	if (!WeaponInstigator)
	{
		WeaponInstigator = Cast<APawn>(GetOwner());
	}
	if (!WeaponInstigator)
	{
		WeaponInstigator = GetInstigator<APawn>();
	}

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (HitPawn == WeaponInstigator)
		{
			return;
		}

		if (WeaponInstigator && UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponInstigator, HitPawn))
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}

