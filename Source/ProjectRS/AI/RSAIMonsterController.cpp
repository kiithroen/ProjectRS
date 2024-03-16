// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RSAIMonsterController.h"

#include "Character/RSHero.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

ARSAIMonsterController::ARSAIMonsterController()
{
}

void ARSAIMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	GetWorldTimerManager().SetTimer(AITimerHandle, this, &ARSAIMonsterController::OnUpdateAI, 10.f / 60.f, true);
}

void ARSAIMonsterController::OnUnPossess()
{
	Super::OnUnPossess();
	
	ClearAITimer();
}

void ARSAIMonsterController::OnUpdateAI()
{
	APawn* OwnerPawn = GetPawn();
	if (!IsValid(OwnerPawn))
		return;
	
	IRSCombatInterface* OwnerCombat = Cast<IRSCombatInterface>(OwnerPawn);
	if (!OwnerCombat)
		return;
	
	if (OwnerCombat->IsDead())
		return;
	
	ARSHero* Hero = Cast<ARSHero>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Hero))
		return;

	UPathFollowingComponent* PathFollowingComp = GetPathFollowingComponent();
	if (!PathFollowingComp)
		return;

	constexpr float MeleeAttackRange = 150.f;
	if (FVector::DistSquared2D(OwnerPawn->GetActorLocation(), Hero->GetActorLocation()) <= MeleeAttackRange * MeleeAttackRange)
	{
		StopMovement();
		OwnerCombat->UseSkillSlot(RSGT_Skill_Slot_MeleeAttack_1);
	}
	else
	{
		if (PathFollowingComp->GetStatus() == EPathFollowingStatus::Idle)
		{
			MoveToActor(Hero, 50.f);
		}
	}
}


void ARSAIMonsterController::ClearAITimer()
{
	if (AITimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(AITimerHandle);
		AITimerHandle.Invalidate();
	}
}
