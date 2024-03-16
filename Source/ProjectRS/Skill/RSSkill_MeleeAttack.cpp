// Fill out your copyright notice in the Description page of Project Settings.


#include "RSSkill_MeleeAttack.h"
#include "Actor/RSProjectile.h"
#include "Skill/RSSkillEffect.h"
#include "RSType.h"
#include "Common/RSUtil.h"
#include "Character/RSMonster.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/KismetMathLibrary.h"

URSSkill_MeleeAttack::URSSkill_MeleeAttack()
{
}

void URSSkill_MeleeAttack::OnBegin()
{
	Super::OnBegin();

	bTraceMeleeAttack = false;
	ActorsToIgnore.Reset();
	
	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	if (IRSCombatInterface* OwnerCombat = Cast<IRSCombatInterface>(OwnerActor))
	{
		OwnerCombat->EnableMovementInput(false);
	}
	
	if (IRSAnimInterface* OwnerAnim = Cast<IRSAnimInterface>(OwnerActor))
	{
		if (UAnimMontage* Montage = MontageToPlay.LoadSynchronous())
		{
			OwnerAnim->PlayMontageWithEnd(Montage, [OwnerAnim, this](UAnimMontage*,bool)
			{
				EndSkill();
			});
		}
		else
		{
			EndSkill();
		}
	}
}

void URSSkill_MeleeAttack::Do(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_SkillMeleeAttackDo);
	
	Super::Do(DeltaTime);

	if (!bTraceMeleeAttack)
		return;
	
	TArray<FHitResult> OutHitResults;
	FRSTargetInfo_SocketTrace TargetInfo;
	TargetInfo.StartSocket = StartSocket;
	TargetInfo.EndSocket = EndSocket;
	TargetInfo.Radius = Radius;
	TargetInfo.TraceChannel = TraceChannel;

	URSUtil::CollectTargets_SocketTrace(GetOwnerActor(), TargetInfo, ActorsToIgnore, OutHitResults);

	if (OutHitResults.Num() > 0)
	{
		for (const auto& HitResult : OutHitResults)
		{
			const AActor* HitActor = HitResult.GetActor();
			if (!IsValid(HitActor))
				continue;
			
			// 두번 맞지 않게
			ActorsToIgnore.AddUnique(HitResult.GetActor());
			
			URSSkillComponent* SkillComp = HitActor->FindComponentByClass<URSSkillComponent>();
			if (!SkillComp)
				continue;

			if (SkillComp->HasFlag(RSGT_Flag_Dead))
				continue;

			for (auto& SkillEffect : SkillEffectOnHit)
			{
				SkillComp->AddSkillEffect(SkillEffect, GetLevel(), GetOwnerActor(), nullptr, &HitResult);
			}
		}
	}
}

void URSSkill_MeleeAttack::OnEnd()
{
	Super::OnEnd();
	
	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	if (IRSCombatInterface* OwnerCombat = Cast<IRSCombatInterface>(OwnerActor))
	{
		OwnerCombat->EnableMovementInput(true);
	}
}

void URSSkill_MeleeAttack::OnEvent(const FGameplayTag& EventTag)
{
	Super::OnEvent(EventTag);

	if (EventTag.MatchesTagExact(RSGT_Skill_AnimNotify_ApplySkill_Begin))
	{
		bTraceMeleeAttack = true;
		ActorsToIgnore.Reset();
	}
	else if (EventTag.MatchesTagExact(RSGT_Skill_AnimNotify_ApplySkill_End))
	{
		bTraceMeleeAttack = false;
		ActorsToIgnore.Reset();
	}
}
