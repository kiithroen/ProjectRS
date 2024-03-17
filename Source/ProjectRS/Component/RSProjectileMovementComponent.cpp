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
	
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
    {
    	PrimaryComponentTick.UnRegisterTickFunction();
    	AggregatingTickSubsystem->RegisterComponent(this, TG_PrePhysics);
    }
}

void URSProjectileMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
	{
		AggregatingTickSubsystem->UnRegisterComponent(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void URSProjectileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProjectileMovementTick);
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
