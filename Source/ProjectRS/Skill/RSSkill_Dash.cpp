// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/RSSkill_Dash.h"
#include "Character/RSCharacter.h"

void URSSkill_Dash::OnBegin()
{
	Super::OnBegin();

	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	if (IRSCombatInterface* OwnerCombat = Cast<IRSCombatInterface>(OwnerActor))
	{
		const FVector DashDir = OwnerCombat->GetLastMovementDirection();
		OwnerCombat->EnableMovementInput(false);
		OwnerCombat->StopMovementAll();
		float Duration = OwnerCombat->StartMovementByCurve(DashCurve, DashDir, DashCurveScale);
		if (Duration > 0.f)
		{
			FTimerHandle TimerHandle;;
			OwnerActor->GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				EndSkill();
			}), Duration, false);
		}
		else
		{
			ensure(false);
			EndSkill();
		}
		
		if (bGhostMode)
		{
			OwnerCombat->EnableGhost(true);
		}
	}
	
	if (IRSAnimInterface* OwnerAnim = Cast<IRSAnimInterface>(OwnerActor))
	{
		if (UAnimMontage* Montage = MontageToPlay.LoadSynchronous())
		{
			OwnerAnim->PlayMontage(Montage);
		}
	}
}

void URSSkill_Dash::OnEnd()
{
	Super::OnEnd();

	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	if (IRSCombatInterface* OwnerCombat = Cast<IRSCombatInterface>(OwnerActor))
	{
		if (bGhostMode)
		{
			OwnerCombat->EnableGhost(false);
		}

		OwnerCombat->EnableMovementInput(true);
	}
	
	if (IRSAnimInterface* OwnerAnim = Cast<IRSAnimInterface>(OwnerActor))
	{
		if (UAnimMontage* Montage = MontageToPlay.LoadSynchronous())
		{
			OwnerAnim->StopMontage(Montage);
		}
	}
}