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

    /** Player **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Light_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Heavy_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_HitPause);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_HitPause);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_JumpToFinisher);
	
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
	
}
