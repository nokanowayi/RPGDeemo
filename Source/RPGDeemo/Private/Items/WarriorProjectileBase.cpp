// MyRPGDemo


#include "Items/WarriorProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"

// Sets default values
AWarriorProjectileBase::AWarriorProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// 碰撞盒作为根组件
	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);

	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	// 先全部 Ignore 清底，再单独开启目标通道，避免默认预设干扰
	ProjectileCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	// Overlap 事件必须有一方开启 GenerateOverlapEvents 才会触发
	ProjectileCollisionBox->SetGenerateOverlapEvents(true);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	// 特效组件挂在碰撞盒上
	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 700.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;
}

void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	if (DamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	}

}

void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	// 防止打到自己直接忽略
	if (OtherActor == GetInstigator() || OtherActor == GetOwner())
	{
		return;
	}

	APawn* HitPawn = Cast<APawn>(OtherActor);
	
	// 撞到非 Pawn直接销毁
	if (!HitPawn)
	{
		Destroy();
		return;
	}

	// 同阵营 Pawn忽略，而不是把火球销毁，同时加入移动忽略列表，保证后续 sweep 也不会被它挡住
	if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		ProjectileCollisionBox->IgnoreActorWhenMoving(HitPawn, true);
		return;
	}
	
    bool bIsValidBlock = false;
	const bool bIsPlayerBlock = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn,WarriorGameplayTags::Player_Status_Blocking);

	if (bIsPlayerBlock)
	{
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this,HitPawn);
	}

	FGameplayEventData Data;
	Data.Instigator = GetInstigator();
	Data.Target = HitPawn;
	
	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitPawn, WarriorGameplayTags::Player_Event_SuccessfulBlock, Data);
		Destroy();
		return;
	}
	else
	{
		HandleApplyProjectileDamage(HitPawn,Data);
	}

	Destroy();
}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator())
	{
		return;
	}

	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	OverlappedActors.AddUnique(OtherActor);

	if(APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		// 同阵营接忽略，不结算也不销毁火球，并加入移动忽略列表
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			ProjectileCollisionBox->IgnoreActorWhenMoving(HitPawn, true);
			return;
		}

		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		HandleApplyProjectileDamage(HitPawn,Data);

		
		if (DamagePolicy == EProjectileDamagePolicy::OnHit && HasAuthority())
		{
			Destroy();
		}
	}
	
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& Data)
{
	
	if (!HasAuthority())
	{
		return;
	}

	
	if (!ProjectileDamageEffectSpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Projectile] 伤害规格为空！命中=%s 发射者=%s NetMode=%d —— 检查投射物 SpawnActor 节点的 ")
			TEXT("ProjectileDamageEffectSpecHandle 引脚，以及上游 MakeOutgoingGameplayEffectSpec 的 GameplayEffectClass"),
			*GetNameSafe(InHitPawn), *GetNameSafe(GetInstigator()), (int32)GetNetMode());
		return;
	}

	const bool bWasSuccessful = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);
	if (bWasSuccessful)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InHitPawn, WarriorGameplayTags::Shared_Event_HitReact, Data);
	}
}


