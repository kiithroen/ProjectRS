// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/RSSkillEffect_Heal.h"
#include "Component/RSStatComponent.h"
#include "Skill/RSSkillComponent.h"
#include "RSType.h"
#include "Character/RSCharacter.h"
#include "Common/RSUtil.h"

URSSkillEffect_Heal::URSSkillEffect_Heal()
{
}

void URSSkillEffect_Heal::Do(float DeltaTime)
{
	Super::Do(DeltaTime);

	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	IRSCombatInterface* OwnerCombat = Cast<ARSCharacter>(OwnerActor);
	if (!OwnerCombat)
		return;
	
	if (OwnerCombat->IsDead())
		return;

	constexpr float MaxHeal = FLT_MAX;
	const float FinalHeal = BaseHeal.GetValueAtLevel(GetLevel());
	const float RandomHeal = FinalHeal * URSUtil::MTFRandRange(-RandomHealMultiplier, RandomHealMultiplier);

	const float RealHeal = static_cast<float>(FMath::TruncToInt(FMath::Clamp(FinalHeal + RandomHeal, 1.f, MaxHeal)));
	
	OwnerCombat->ApplyHeal(RealHeal, GetCaster());
}
