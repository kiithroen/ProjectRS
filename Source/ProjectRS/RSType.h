// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "NativeGameplayTags.h"
#include "RSType.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRS, Log, All);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Flag_Dead);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Flag_SuperArmor);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Flag_Immune);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_CurrentHP);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_MaxHP);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_CurrentMana);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_MaxMana);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_AttackDamage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_AttackSpeed);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_CriticalChance);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_Armor);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_ArmorPenetration);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Stat_MovementSpeed);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_Id_Null);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_Type_Default);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_Type_Sub);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_Event_Input_ButtonA);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Id_Null);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Type_Passive);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Type_Active);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Type_Buff);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Type_Debuff);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Type_CC);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Type_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_Type_Heal);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_SkillEffect_StackGroup_Null);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Character_Id_Null);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Item_Id_Null);

#define ECC_OBJECT_HITBOX ECC_GameTraceChannel1
#define ECC_TRACE_HITTRACE ECC_GameTraceChannel2

constexpr float MaxDamage = 99999999;	// 현재는 1억 이하의 데미지만 고려

struct FRSName
{
	static const FName MatParam_HitSplash;
};

class UNiagaraSystem;

UENUM(BlueprintType)
enum class ERSTeamId : uint8
{
	Player UMETA(DisplayName = "Player"),
	Monster UMETA(DisplayName = "Monster"),
	Neutral UMETA(DisplayName = "Neutral"),
};

UENUM(Blueprintable)
enum class ERSSkillEffectExecutionType : uint8
{
	Once,
	Repeat,
	Infinite,
};

UENUM(Blueprintable)
enum class ERSFieldItemType : uint8
{
	HealthPotion,
	Bomb,
};

namespace RSTeamType
{
	const static FGenericTeamId Neutral = FGenericTeamId(uint8(ERSTeamId::Neutral));
	const static FGenericTeamId Monster = FGenericTeamId(uint8(ERSTeamId::Monster));
	const static FGenericTeamId Player = FGenericTeamId(uint8(ERSTeamId::Player));

	ETeamAttitude::Type Resolve(const AActor* LeftActor, const AActor* RightActor);
}

USTRUCT(BlueprintType)
struct FRSParticleInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	TObjectPtr<UNiagaraSystem> NSTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FVector Scale3D = FVector::OneVector;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	bool bAttached = false;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FName SocketName;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	int32 TranslucencySortPriority = 0;
};