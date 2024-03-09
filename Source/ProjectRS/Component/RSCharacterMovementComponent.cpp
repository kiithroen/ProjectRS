// Fill out your copyright notice in the Description page of Project Settings.


#include "RSCharacterMovementComponent.h"

#include "System/RSAggregatingTickSubsystem.h"


URSCharacterMovementComponent::URSCharacterMovementComponent()
{
	bProjectNavMeshWalking = true;
	NavMeshProjectionInterval = 1.f;
	NavMeshProjectionInterpSpeed = 0.f;
}

void URSCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	const UWorld* World = GetWorld();
	if (!World)
		return;
	
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = World->GetSubsystem<URSAggregatingTickSubsystem>())
	{
		PrimaryComponentTick.UnRegisterTickFunction();
		AggregatingTickSubsystem->RegisterComponent(this, TG_PostPhysics);
	}
	
}

void URSCharacterMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const UWorld* World = GetWorld();
	if (!World)
		return;
	
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = World->GetSubsystem<URSAggregatingTickSubsystem>())
	{
		AggregatingTickSubsystem->UnRegisterComponent(this);
	}
	
	Super::EndPlay(EndPlayReason);
}