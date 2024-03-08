// Fill out your copyright notice in the Description page of Project Settings.


#include "RSType.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogRS);

UE_DEFINE_GAMEPLAY_TAG(RSGT_Flag_Dead, "Flag.Dead");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Flag_SuperArmor, "Flag.SuperArmor");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Flag_Immune, "Flag.Immune");

UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_CurrentHP, "Stat.CurrentHP");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_MaxHP, "Stat.MaxHP");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_CurrentMana, "Stat.CurrentMana");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_MaxMana, "Stat.MaxMana");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_AttackDamage, "Stat.AttackDamage");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_AttackSpeed, "Stat.AttackSpeed");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_CriticalChance, "Stat.CriticalChance");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_Armor, "Stat.Armor");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_ArmorPenetration, "Stat.ArmorPenetration");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Stat_MovementSpeed, "Stat.MovementSpeed");

UE_DEFINE_GAMEPLAY_TAG(RSGT_Skill_Id_Null, "Skill.Id.Null");

UE_DEFINE_GAMEPLAY_TAG(RSGT_Skill_Type_Default, "Skill.Type.Default");
UE_DEFINE_GAMEPLAY_TAG(RSGT_Skill_Type_Sub, "Skill.Type.Sub");

UE_DEFINE_GAMEPLAY_TAG(RSGT_Skill_Event_Input_ButtonA, "Skill.Event.Input.ButtonA");

UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Id_Null, "SkillEffect.Id.Null");

UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Type_Passive, "SkillEffect.Type.Passive");
UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Type_Active, "SkillEffect.Type.Active");
UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Type_Buff, "SkillEffect.Type.Buff");
UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Type_Debuff, "SkillEffect.Type.Debuff");
UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Type_CC, "SkillEffect.Type.CC");
UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Type_Damage, "SkillEffect.Type.Damage");
UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_Type_Heal, "SkillEffect.Type.Heal");

UE_DEFINE_GAMEPLAY_TAG(RSGT_SkillEffect_StackGroup_Null, "SkillEffect.StackGroup.Null");

UE_DEFINE_GAMEPLAY_TAG(RSGT_Character_Id_Null, "Character.Id.Null");

const FName FRSName::MatParam_HitSplash = FName(TEXT("HitSplash"));
