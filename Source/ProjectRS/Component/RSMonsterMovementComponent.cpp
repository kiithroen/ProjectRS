// Fill out your copyright notice in the Description page of Project Settings.


#include "RSMonsterMovementComponent.h"

#include "RSType.h"
#include "AI/Navigation/NavigationDataInterface.h"
#include "GameFramework/Character.h"
#include "System/RSAggregatingTickSubsystem.h"
#include "Components/CapsuleComponent.h"

URSMonsterMovementComponent::URSMonsterMovementComponent()
{
	bProjectNavMeshWalking = true;
	NavMeshProjectionInterval = 0.3f;
}

void URSMonsterMovementComponent::BeginPlay()
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

void URSMonsterMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void URSMonsterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URSMonsterMovementComponent::PerformMovement(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_MonsterMovement);
	
	HandlePendingLaunch();
	StartNewPhysics(DeltaTime, 0);
	PhysicsRotation(DeltaTime);
	LastUpdateRequestedVelocity = bHasRequestedVelocity ? RequestedVelocity : FVector::ZeroVector;
	bHasRequestedVelocity = false;
	UpdateComponentVelocity();
	
	LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	LastUpdateRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
	LastUpdateVelocity = Velocity;
}

void URSMonsterMovementComponent::PhysNavWalking(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, GroundFriction, false, BrakingDecelerationWalking);
	
	if (IsFalling())
    {
    	StartNewPhysics(DeltaTime, Iterations);
    	return;
    }

	Iterations++;

	FVector DesiredMove = Velocity;
	DesiredMove.Z = 0.f;

	const FVector OldLocation = GetActorFeetLocation();
	const FVector DeltaMove = DesiredMove * DeltaTime;
	const bool bDeltaMoveNearlyZero = DeltaMove.IsNearlyZero();

	FVector AdjustedDest = OldLocation + DeltaMove;
	
	FNavLocation DestNavLocation;

	bool bSameNavLocation = false;
	if (CachedNavLocation.NodeRef != INVALID_NAVNODEREF)
	{
		if (bProjectNavMeshWalking)
		{
			const float DistSq2D = (OldLocation - CachedNavLocation.Location).SizeSquared2D();
			const float DistZ = FMath::Abs(OldLocation.Z - CachedNavLocation.Location.Z);

			const float TotalCapsuleHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f;
			const float ProjectionScale = (OldLocation.Z > CachedNavLocation.Location.Z) ? NavMeshProjectionHeightScaleUp : NavMeshProjectionHeightScaleDown;
			const float DistZThr = TotalCapsuleHeight * FMath::Max(0.f, ProjectionScale);

			bSameNavLocation = (DistSq2D <= UE_KINDA_SMALL_NUMBER) && (DistZ < DistZThr);
		}
		else
		{
			bSameNavLocation = CachedNavLocation.Location.Equals(OldLocation);
		}

		if (bDeltaMoveNearlyZero && bSameNavLocation)
		{
			if (const INavigationDataInterface* NavData = GetNavData())
			{
				if (!NavData->IsNodeRefValid(CachedNavLocation.NodeRef))
				{
					CachedNavLocation.NodeRef = INVALID_NAVNODEREF;
					bSameNavLocation = false;
				}
			}
		}
	}

	if (bDeltaMoveNearlyZero && bSameNavLocation)
	{
		DestNavLocation = CachedNavLocation;
	}
	else
	{
		if (bSameNavLocation && bProjectNavMeshWalking)
		{
			AdjustedDest.Z = CachedNavLocation.Location.Z;
		}

		const bool bHasNavigationData = FindNavFloor(AdjustedDest, DestNavLocation);
		if (!bHasNavigationData)
		{
			SetMovementMode(MOVE_Walking);
			return;
		}

		CachedNavLocation = DestNavLocation;
	}

	if (DestNavLocation.NodeRef != INVALID_NAVNODEREF)
	{
		FVector NewLocation(AdjustedDest.X, AdjustedDest.Y, DestNavLocation.Location.Z);
		if (bProjectNavMeshWalking)
		{
			const float TotalCapsuleHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f;
			const float UpOffset = TotalCapsuleHeight * FMath::Max(0.f, NavMeshProjectionHeightScaleUp);
			const float DownOffset = TotalCapsuleHeight * FMath::Max(0.f, NavMeshProjectionHeightScaleDown);
			NewLocation = ProjectLocationFromNavMesh(DeltaTime, OldLocation, NewLocation, UpOffset, DownOffset);
		}

		FVector AdjustedDelta = NewLocation - OldLocation;

		if (!AdjustedDelta.IsNearlyZero())
		{
			FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
			FRotator DeltaRot = GetDeltaRotation(DeltaTime);

			FRotator DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRot);
			DesiredRotation.Pitch = 0.f;
			DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
			DesiredRotation.Roll = 0.f;
			
			constexpr float AngleTolerance = 1e-3f;
			if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
			{
				if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
				{
					DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
				}
			}
				
			FHitResult HitResult;
			SafeMoveUpdatedComponent(AdjustedDelta, DesiredRotation, bSweepWhileNavWalking, HitResult);
		}

		if (!bJustTeleported)
		{
			Velocity = (GetActorFeetLocation() - OldLocation) / DeltaTime;
		}

		bJustTeleported = false;
	}
	else
	{
		StartFalling(Iterations, DeltaTime, DeltaTime, DeltaMove, OldLocation);
	}
}
