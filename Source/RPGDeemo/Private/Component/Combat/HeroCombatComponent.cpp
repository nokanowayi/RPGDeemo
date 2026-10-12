// MyRPGDemo


#include "Component/Combat/HeroCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "Items/Weapons/WarriorHeroWeapon.h"

namespace
{
	constexpr float HitStopTimeDilation = 0.1f;
	constexpr float HitStopDuration = 0.2f;
	
	void ApplyHitStopToOwningPawn(APawn* InPawn)
	{
		if (!InPawn)
		{
			return;
		}
		
		if (!InPawn->IsLocallyControlled() && !InPawn->HasAuthority())
		{
			return;
		}

		InPawn->CustomTimeDilation = HitStopTimeDilation;

		if (UWorld* World = InPawn->GetWorld())
		{
			const TWeakObjectPtr<APawn> WeakPawn(InPawn);

			// 局部 handle ：恢复动作是幂等的（设回 1.0），不需要主动取消
			FTimerHandle RestoreTimerHandle;
			World->GetTimerManager().SetTimer(
				RestoreTimerHandle,
				FTimerDelegate::CreateWeakLambda(InPawn, [WeakPawn]()
				{
					if (APawn* Pawn = WeakPawn.Get())
					{
						Pawn->CustomTimeDilation = 1.f;
					}
				}),
				HitStopDuration,
				false);
		}
	}
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return  Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
	return  Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
{
	return  GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlapActors.Contains(HitActor))
	{
		return;
	}
	OverlapActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(),WarriorGameplayTags::Shared_Event_MeleeHit,Data);

	// 顿帧：只减缓攻击者本体。
	// 注意：这里不再发送 Player.Event.HitPause（那个事件会激活 GA_Hero_HitPause，
	// 而它内部用的是全局时间膨胀，多人下会打架且可能永久卡住，详见本文件顶部注释）。
	ApplyHitStopToOwningPawn(GetOwningPawn());
}

void UHeroCombatComponent::OnWeaponPullFromTargetActor(AActor* /*InteractedActor*/)
{
	// 拔武器时的顿帧，同样只作用于本体
	ApplyHitStopToOwningPawn(GetOwningPawn());
}
