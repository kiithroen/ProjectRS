// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "RSType.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRS, Log, All);

DECLARE_STATS_GROUP(TEXT("RS"), STATGROUP_RS, STATCAT_Advanced);
DECLARE_STATS_GROUP(TEXT("RSSKILL"), STATGROUP_RSSKILL, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("Skill [Tick]"), STAT_SkillTick, STATGROUP_RS);
DECLARE_CYCLE_STAT(TEXT("Projectile [Tick]"), STAT_ProjectileTick, STATGROUP_RS);
DECLARE_CYCLE_STAT(TEXT("Projectile [Spawn]"), STAT_ProjectileSpawn, STATGROUP_RS);
DECLARE_CYCLE_STAT(TEXT("FieldItem [Tick]"), STAT_FieldItemTick, STATGROUP_RS);
DECLARE_CYCLE_STAT(TEXT("HeroMovement [Tick]"), STAT_HeroMovementTick, STATGROUP_RS);
DECLARE_CYCLE_STAT(TEXT("MonsterMovement [Tick]"), STAT_MonsterMovementTick, STATGROUP_RS);
DECLARE_CYCLE_STAT(TEXT("ProjectileMovement [Tick]"), STAT_ProjectileMovementTick, STATGROUP_RS);

DECLARE_CYCLE_STAT(TEXT("RangeAttack [Do]"), STAT_SkillRangeAttackDo, STATGROUP_RSSKILL);
DECLARE_CYCLE_STAT(TEXT("MeleeAttack [Do]"), STAT_SkillMeleeAttackDo, STATGROUP_RSSKILL);

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

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_Slot_Input_BttonA);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_Slot_MeleeAttack_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_Slot_RangeAttack_1);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_3);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_4);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_5);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_Begin);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_Tick);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(RSGT_Skill_AnimNotify_ApplySkill_End);

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

struct PROJECTRS_API FRSName
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

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSParticleInfo
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


USTRUCT(BlueprintType)
struct PROJECTRS_API FRSTargetInfo_SocketTrace
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	FName StartSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	FName EndSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float Radius = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
};

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSTargetInfo_BoxTrace
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	FVector StartOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float Distance = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	FVector HalfSize = FVector(100.f, 100.f, 100.f);;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
};

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSTargetInfo_BoxArea
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector EndLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	FVector HalfSize = FVector(100.f, 100.f, 100.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
};

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSTargetInfo_SphereArea
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector EndLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float Radius = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
};


USTRUCT(BlueprintType)
struct PROJECTRS_API FRSTargetInfo_CylinderArea
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector EndLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float Radius = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
};

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSTargetInfo_CapsuleArea
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	FVector EndLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float Radius = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
};

