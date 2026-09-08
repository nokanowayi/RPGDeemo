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

	if (DamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	}

}

void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	// 防止打到自己：Instigator 或 Owner 是自己都直接忽略
	if (OtherActor == GetInstigator() || OtherActor == GetOwner())
	{
		return;
	}

	APawn* HitPawn = Cast<APawn>(OtherActor);
	
	if (!HitPawn||UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			// 调试：打印命中者、发射者、双方队伍 ID（-1 = 无队伍接口）
			const IGenericTeamAgentInterface* InstigatorTeam = GetInstigator() ? Cast<const IGenericTeamAgentInterface>(GetInstigator()->GetController()) : nullptr;
			const IGenericTeamAgentInterface* TargetTeam = Cast<const IGenericTeamAgentInterface>(HitPawn->GetController());
		}
		Destroy();
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
		if (!UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			FGameplayEventData Data;
			Data.Instigator = GetInstigator();
			Data.Target = HitPawn;

			HandleApplyProjectileDamage(HitPawn,Data);
		}
	}
	
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& Data)
{
	const bool bWasSuccessful = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);
	if (bWasSuccessful)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InHitPawn, WarriorGameplayTags::Shared_Event_HitReact, Data);
	}
}


