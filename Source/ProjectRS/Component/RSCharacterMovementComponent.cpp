// Fill out your copyright notice in the Description page of Project Settings.


#include "RSCharacterMovementComponent.h"

#include "RSType.h"
#include "Curves/CurveVector.h"

URSCharacterMovementComponent::URSCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URSCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URSCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateLastMovementDirection();
}

void URSCharacterMovementComponent::UpdateLastMovementDirection()
{
	LastMovementDirection = Velocity.GetSafeNormal();
	if (LastMovementDirection.IsNearlyZero())
	{
		LastMovementDirection = GetLastMovementDirection();
		if (LastMovementDirection.IsNearlyZero())
		{
			LastMovementDirection = UpdatedComponent ? UpdatedComponent->GetForwardVector() : FVector::ZeroVector;
		}
	}
}

void URSCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	if (bMovementByCurve)
	{
		PerformMovementByCurve(DeltaTime);
		return;
	}
	
	Super::PerformMovement(DeltaTime);
}

void URSCharacterMovementComponent::StopMovementAll()
{
	StopMovementByCurve();
	StopMovementImmediately();
}

float URSCharacterMovementComponent::StartMovementByCurve(UCurveVector* Curve, const FVector& Direction, const FVector& Scale)
{
	if (!Curve)
	{
		ensure(false);
		return 0.f;
	}

	bMovementByCurve = true;
	MovementByCurveElapsedTime = 0.f;
	MovementCurve = Curve;
	MovementCurveDirection = Direction;
	MovementCurveScale = Scale;

	float MinTime = 0.f;
	float MaxTime = 0.f;
	MovementCurve->GetTimeRange(MinTime, MaxTime);
	
	return MaxTime;
}

void URSCharacterMovementComponent::StopMovementByCurve()
{
	bMovementByCurve = false;
	MovementByCurveElapsedTime = 0.f;
	MovementCurve = nullptr;
	MovementCurveDirection = FVector::ForwardVector;
	MovementCurveScale = FVector::ZeroVector;
}

void URSCharacterMovementComponent::PerformMovementByCurve(float DeltaTime)
{
	ClearAccumulatedForces();
	
	UpdateMovementByCurve(DeltaTime);
	
	LastUpdateRequestedVelocity = bHasRequestedVelocity ? RequestedVelocity : FVector::ZeroVector;
	bHasRequestedVelocity = false;
	UpdateComponentVelocity();
	
	LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	LastUpdateRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
	LastUpdateVelocity = Velocity;
}

void URSCharacterMovementComponent::UpdateMovementByCurve(float DeltaTime)
{
	if (!bMovementByCurve)
		return;

	if (!MovementCurve)
		return;
	
	MovementByCurveElapsedTime += DeltaTime;

	float MinTime = 0.f;
	float MaxTime = 0.f;
	MovementCurve->GetTimeRange(MinTime, MaxTime);

	if (MovementByCurveElapsedTime >= MaxTime)
	{
		StopMovementByCurve();
		return;
	}

	const FVector MovementCurveVelocity = MovementCurve->GetVectorValue(MovementByCurveElapsedTime) * MovementCurveScale;
	const FTransform ToDirection(MovementCurveDirection.Rotation());
	const FVector DesiredVelocity = ToDirection.TransformVectorNoScale(MovementCurveVelocity);
	
	Velocity = DesiredVelocity;

	const FVector DeltaLocation = DesiredVelocity * DeltaTime;

	FHitResult HitResult;
	SafeMoveUpdatedComponent(DeltaLocation, DesiredVelocity.Rotation(), true, HitResult);
	if (HitResult.bBlockingHit)
	{
		StopMovementByCurve();
	}
}
