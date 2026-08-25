// MyRPGDemo

#pragma once

#include "NativeGameplayTags.h"

namespace WarriorGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Equip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Unequip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LightAttack_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_HeavyAttack_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwitchTarget);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld_Block);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable_TargetLock);
    /** Player **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Light_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Heavy_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_HitPause);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Roll);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Block);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_TargetLock);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_HitPause);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SuccessfulBlock);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SwitchTarget_Left);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SwitchTarget_Right);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_JumpToFinisher);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rolling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Blocking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_TargetLocking);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Heavy);
	
	/** Enemy **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Melee);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Ranged);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Weapon);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_Strafing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_UnderAttack);

	/** Shared Tags **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_HitReact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_Death);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_MeleeHit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_HitReact);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_SetByCaller_BaseDamage);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Front);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Back);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Left);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Right);
	
}
