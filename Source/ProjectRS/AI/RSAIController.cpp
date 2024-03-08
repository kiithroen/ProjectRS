// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/RSAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"

ARSAIController::ARSAIController()
{
}

void ARSAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ARSAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetActorTickEnabled(true);
}

void ARSAIController::OnUnPossess()
{
	Super::OnUnPossess();

	SetActorTickEnabled(false);
}