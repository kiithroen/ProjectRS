// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/RSSkillEffect_Heal.h"
#include "Component/RSStatComponent.h"
#include "Skill/RSSkillComponent.h"
#include "RSType.h"
#include "Character/RSCharacter.h"
#include "Common/RSObjectPool.h"
#include "Common/RSUtil.h"
#include "GameFramework/RSAssetManager.h"

URSSkillEffect* URSSkillEffect_Heal::Clone() const
{
	if (URSSkillEffect_Heal* CloneSkillEffect = URSAssetManager::Get().GetObjectPool().Pop<URSSkillEffect_Heal>())
	{
		CloneSkillEffect->CopyFrom(this);
		return CloneSkillEffect;
	}

	ensure(false);
	return nullptr;
}

void URSSkillEffect_Heal::Release()
{
	URSAssetManager::Get().GetObjectPool().Push<URSSkillEffect_Heal>(this);
}

void URSSkillEffect_Heal::CopyFrom(const URSSkillEffect* Other)
{
	Super::CopyFrom(Other);

	if (const URSSkillEffect_Heal* OtherDamage = Cast<const URSSkillEffect_Heal>(Other))
	{
		BaseHeal = OtherDamage->BaseHeal;
		RandomHealMultiplier = OtherDamage->RandomHealMultiplier;
	}
}

void URSSkillEffect_Heal::Do(float DeltaTime)
{
	Super::Do(DeltaTime);

	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	IRSCombatInterface* OwnerCombat = Cast<IRSCombatInterface>(OwnerActor);
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
