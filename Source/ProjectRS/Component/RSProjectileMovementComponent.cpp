// Fill out your copyright notice in the Description page of Project Settings.


#include "RSProjectileMovementComponent.h"

#include "RSType.h"
#include "System/RSAggregatingTickSubsystem.h"


URSProjectileMovementComponent::URSProjectileMovementComponent()
{
}

void URSProjectileMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URSProjectileMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void URSProjectileMovementComponent::BeginPlayFromPool()
{
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
	{
		PrimaryComponentTick.UnRegisterTickFunction();
		AggregatingTickSubsystem->RegisterComponent(this, TG_PrePhysics);
	}
}

void URSProjectileMovementComponent::EndPlayFromPool()
{
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
	{
		AggregatingTickSubsystem->UnRegisterComponent(this);
	}
}

void URSProjectileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProjectileMovementTick);
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
