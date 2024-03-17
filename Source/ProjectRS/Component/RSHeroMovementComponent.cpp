// Fill out your copyright notice in the Description page of Project Settings.


#include "RSHeroMovementComponent.h"

#include "RSType.h"
#include "System/RSAggregatingTickSubsystem.h"

URSHeroMovementComponent::URSHeroMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSHeroMovementComponent::OnSpawn()
{
	Super::OnSpawn();
	
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
	{
		PrimaryComponentTick.UnRegisterTickFunction();
		AggregatingTickSubsystem->RegisterComponent(this, TG_PrePhysics);
	}
}

void URSHeroMovementComponent::OnDespawn()
{
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
	{
		AggregatingTickSubsystem->UnRegisterComponent(this);
	}

	Super::OnDespawn();
}

void URSHeroMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_HeroMovementTick);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
