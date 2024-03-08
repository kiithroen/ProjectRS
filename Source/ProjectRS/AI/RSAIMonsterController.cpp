// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RSAIMonsterController.h"

ARSAIMonsterController::ARSAIMonsterController()
{
	PrimaryActorTick.bCanEverTick = false;
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
}