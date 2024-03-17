// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/RSAIController.h"

ARSAIController::ARSAIController()
{
	PrimaryActorTick.bCanEverTick = false;
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
}

void ARSAIController::OnUnPossess()
{
	Super::OnUnPossess();
}