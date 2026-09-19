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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SpecialWeaponAbility_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SpecialWeaponAbility_Heavy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Interact);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld_Block);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable_TargetLock);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable_Rage);
    /** Player **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Light_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Heavy_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_HitPause);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Roll);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Block);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_TargetLock);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Rage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_SpecialWeaponAbility_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_SpecialWeaponAbility_Heavy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Interact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Contact_Stones);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_CoolDown_SpecialWeaponAbility_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_CoolDown_SpecialWeaponAbility_Heavy);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_HitPause);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SuccessfulBlock);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SwitchTarget_Left);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SwitchTarget_Right);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_ActivateRage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_AOE);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_ConsumeStone);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_JumpToFinisher);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rolling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Blocking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_TargetLocking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_Activating);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_Active);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_Full);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_None);

	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Heavy);
	
	/** Enemy **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Melee);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Ranged);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_SummonEnemies);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_SpawnStone);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Weapon);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Event_SummonEnemies);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_Strafing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_UnderAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_UnBlockable);

	/** UI **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_Modal);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_GameMenu);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_GameHud);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_Frontend);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_PressAnyKeyScreen);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_MainMenuScreen);

	/** Shared Tags **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_HitReact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_Death);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_MeleeHit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_HitReact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_SpawnProjectile);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_SetByCaller_BaseDamage);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Front);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Back);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Left);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Right);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Invincible);

	/** Game Data Tag**/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameData_Level_SurvivalGameModeMap);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameData_Level_MainMenuMap);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameData_SaveGame_Slot_1);
	
}
