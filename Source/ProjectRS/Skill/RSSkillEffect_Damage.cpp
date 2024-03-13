// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/RSSkillEffect_Damage.h"
#include "Component/RSStatComponent.h"
#include "Skill/RSSkillComponent.h"
#include "Character/RSCharacter.h"
#include "Common/RSUtil.h"
#include "Kismet/KismetMathLibrary.h"

void URSSkillEffect_Damage::Do(float DeltaTime)
{
	Super::Do(DeltaTime);

	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;
	
	IRSCombatInterface* OwnerCombat = Cast<ARSCharacter>(OwnerActor);
	if (!OwnerCombat)
		return;

	if (OwnerCombat->IsDead() || OwnerCombat->IsImmune())
		return;

	const float Armor = FMath::Max(0, OwnerCombat->GetStat(RSGT_Stat_Armor));
	const float DamageReduction = bIgnoreArmor ? 1.f : (100.f / (100.f + Armor));
	
	float FinalDamage;
	
	if (IRSCombatInterface* CasterCombat = Cast<ARSCharacter>(GetCaster()))
	{
		const float AttackDamage = FMath::Max(1, CasterCombat->GetStat(RSGT_Stat_AttackDamage));
		const float CalcDamage = (BaseDamage.GetValueAtLevel(GetLevel()) + AttackDamage * AttackDamageMultiplier.GetValueAtLevel(GetLevel())) * DamageReduction;

		bool bCritical = false;
		if (bAlwaysCritical)
		{
			bCritical = true;
		}
		else
		{
			const int32 RandomValue = URSUtil::MTRandRange(1, 10000);
			const int32 CriticalChance = static_cast<int32>(CasterCombat->GetStat(RSGT_Stat_CriticalChance));
			if (RandomValue <= CriticalChance)
			{
				bCritical = true;
			}
		}

		float CriticalDamage = 0.f;
		if (bCritical)
		{
			CriticalDamage = CalcDamage * 0.75f;
		}

		FinalDamage = CalcDamage + CriticalDamage;
	}
	else
	{
		FinalDamage = BaseDamage.GetValueAtLevel(GetLevel()) * DamageReduction;
	}

	const float RandomDamage = FinalDamage * URSUtil::MTFRandRange(-RandomDamageMultiplier, RandomDamageMultiplier);
	const float RealDamage = static_cast<float>(FMath::TruncToInt(FMath::Clamp(FinalDamage + RandomDamage, 1.f, MaxDamage)));

	OwnerCombat->ApplyDamage(RealDamage, GetCaster());

	if (const FHitResult* Hit = GetHitResult())
	{
		OwnerCombat->Launch(-Hit->Normal.GetSafeNormal2D() * LaunchSpeed, GetCaster());
	}
}
