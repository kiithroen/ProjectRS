// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RSAIMonsterController.h"
#include "Character/RSMonster.h"
#include "Character/RSHero.h"
#include "Kismet/KismetMathLibrary.h"

ARSAIMonsterController::ARSAIMonsterController()
{
	TeamId = RSTeamType::Monster;
}

void ARSAIMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ARSAIMonsterController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ARSAIMonsterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ARSCharacter* OwnerCharacter = Cast<ARSCharacter>(GetPawn());
	if (!IsValid(OwnerCharacter))
		return;

	if (OwnerCharacter->IsDead())
		return;
	
	const AActor* Hero = GetTopAggro();
	if (!IsValid(Hero))
		return;

	const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(OwnerCharacter->GetActorLocation(), Hero->GetActorLocation());
	OwnerCharacter->AddMovementInput(Direction);
}