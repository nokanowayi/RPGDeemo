// MyRPGDemo


#include "Characters/WarriorBaseCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "MotionWarpingComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));
	WarriorAttributeSet  = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	return GetWarriorAbilitySystemComponent();
}

UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* AWarriorBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}


void AWarriorBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (WarriorAbilitySystemComponent)
	{
		// 服务器端：设置为复制模式
		WarriorAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (WarriorAttributeSet)
		{
			WarriorAbilitySystemComponent->AddAttributeSetSubobject(WarriorAttributeSet);
		}

		// 标记 ASC 已初始化也就是player，敌人没有 PlayerState，靠这个触发客户端初始化
		bAbilitySystemInitialized = true;

		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("CharacterStartUpData is null, please assign CharacterStartUpData in %s"), *GetName());
	}
}

void AWarriorBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 客户端（玩家）初始化
	InitializeAbilitySystemForClient();
}

void AWarriorBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWarriorBaseCharacter, bAbilitySystemInitialized);
}

void AWarriorBaseCharacter::OnRep_AbilitySystemInitialized()
{
	// 客户端（敌人）初始化
	InitializeAbilitySystemForClient();
}

void AWarriorBaseCharacter::InitializeAbilitySystemForClient()
{
	if (!WarriorAbilitySystemComponent) return;
	if (WarriorAbilitySystemComponent->GetAvatarActor() == this) return; // 已初始化

	WarriorAbilitySystemComponent->InitAbilityActorInfo(this, this);

	// 客户端 ASC 初始化后，激活 OnGiven 策略的能力（如 HUD 创建）
	// 服务器在 GiveAbility 时已通过 OnGiveAbility 激活，客户端需要手动补激活
	// 能力列表可能尚未复制完成，延迟几帧重试
	ActivateOnGivenAbilitiesForClient();
}

void AWarriorBaseCharacter::ActivateOnGivenAbilitiesForClient()
{
	if (!WarriorAbilitySystemComponent) return;

	// 立即尝试一次（如果能力已复制到）
	const bool bFoundOnGivenAbility = TryActivateOnGivenAbilities();

	// 如果能力列表还没复制到，延迟重试（最多 5 次，间隔 0.5 秒）
	if (!bFoundOnGivenAbility && OnGivenRetryCount < 5)
	{
		OnGivenRetryCount++;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			this,
			&AWarriorBaseCharacter::ActivateOnGivenAbilitiesForClient,
			0.5f,
			false
		);
	}
}

bool AWarriorBaseCharacter::TryActivateOnGivenAbilities()
{
	if (!WarriorAbilitySystemComponent) return false;

	// HUD 创建依赖 Pawn 已 Possess（否则 GetOwningPlayerPawn 为 NULL）
	// 未 Possess 时返回 false 触发重试
	if (!GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("[HUD] TryActivateOnGivenAbilities: %s 无Controller, 延迟"), *GetName());
		return false;
	}

	bool bFound = false;
	for (const FGameplayAbilitySpec& Spec : WarriorAbilitySystemComponent->GetActivatableAbilities())
	{
		if (const UWarriorGameplayAbility* AbilityCDO = Cast<UWarriorGameplayAbility>(Spec.Ability))
		{
			if (AbilityCDO->IsOnGivenActivationPolicy())
			{
				bFound = true;
				if (!Spec.IsActive())
				{
					UE_LOG(LogTemp, Warning, TEXT("[HUD] %s 激活 OnGiven: %s"), *GetName(), *Spec.Ability->GetName());
					WarriorAbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
				}
			}
		}
	}
	return bFound;
}


