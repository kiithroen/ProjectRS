// Fill out your copyright notice in the Description page of Project Settings.


#include "RSHeroMovementComponent.h"

#include "RSType.h"
#include "System/RSAggregatingTickSubsystem.h"

URSHeroMovementComponent::URSHeroMovementComponent()
{
}

void URSHeroMovementComponent::BeginPlay()
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

void URSHeroMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_HeroMovementTick);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URSHeroMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
