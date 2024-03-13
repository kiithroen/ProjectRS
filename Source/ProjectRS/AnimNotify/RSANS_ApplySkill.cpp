// Fill out your copyright notice in the Description page of Project Settings.


#include "RSANS_ApplySkill.h"

#include "RSType.h"
#include "Skill/RSSkillComponent.h"

void URSANS_ApplySkill::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (IsValid(Owner))
	{
		if (URSSkillComponent* SkillComp = Owner->FindComponentByClass<URSSkillComponent>())
		{
			SkillComp->SendEvent(RSGT_Skill_AnimNotify_ApplySkill_Begin);
		}
	}
}

void URSANS_ApplySkill::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (IsValid(Owner))
	{
		if (URSSkillComponent* SkillComp = Owner->FindComponentByClass<URSSkillComponent>())
		{
			SkillComp->SendEvent(RSGT_Skill_AnimNotify_ApplySkill_End);
		}
	}
}
