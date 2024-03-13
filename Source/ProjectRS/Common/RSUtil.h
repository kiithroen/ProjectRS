// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "RSType.h"
#include "RSUtil.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRS_API URSUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void MTInit();
	static int32 MTRandRange(int32 Min, int32 Max);
	static float MTFRandRange(float Min, float Max);
	static int32 RandomWeightedIndex(const TArray<int32>& Table);
	static FName RandomWeightedName(const TMap<FName, int32>& Table);

	static void CollectTargets_SocketTrace(const AActor* Actor, const FRSTargetInfo_SocketTrace& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults);
	static void CollectTargets_SphereArea(const AActor* Actor, const FRSTargetInfo_SphereArea& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults);
	static void CollectTargets_BoxArea(const AActor* Actor, const FRSTargetInfo_BoxArea& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults);
	static void CollectTargets_CylinderArea(const AActor* Actor, const FRSTargetInfo_CylinderArea& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults);
	
	static float FindAngle2D(const FVector& StartDir, const FVector& TargetDir);

	template <typename T>
	static T* SpawnActor(UWorld* World, UClass* Class, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		if (!World)
			return nullptr;

		if (!Class)
		{
			ensure(false);
			return nullptr;
		}
		
		if (!Class->IsChildOf(T::StaticClass()))
		{
			ensure(false);
			return nullptr;
		}
		
		FActorSpawnParameters SpawnParms;
		SpawnParms.bNoFail = true;
		SpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParms.ObjectFlags |= RF_Transient;

		return World->SpawnActor<T>(Class, SpawnLocation, SpawnRotation, SpawnParms);
	}

	static FOverlapResult FindClosestOverlap(const TArray<FOverlapResult>& OverlapResults, const FVector& Location, float MaxDistance, TFunctionRef<bool(const AActor*)> Filter)
	{
		FOverlapResult ClosestOverlap;
		float ClosestDistanceSq = MaxDistance * MaxDistance;
		for (const auto& OverlapResult : OverlapResults)
		{
			const AActor* Actor = OverlapResult.GetActor();
			if (!IsValid(Actor))
				continue;

			if (!Filter(Actor))
				continue;

			const float DistSq = FVector::DistSquared(Location, Actor->GetActorLocation());
			if (DistSq < ClosestDistanceSq)
			{
				ClosestOverlap = OverlapResult;
				ClosestDistanceSq = DistSq;
			}
		}

		return ClosestOverlap;
	}

	template <typename T>
	static T* FindClosestActor(UWorld* World, const FVector& Location, float MaxDistance, TFunctionRef<bool(const AActor*)> Filter)
	{
		T* ClosestActor = nullptr;
		float ClosestDistanceSq = MaxDistance * MaxDistance;
		for (TActorIterator<T> ActorItr(World); ActorItr; ++ActorItr)
		{
			const T* Actor = *ActorItr;
			if (!IsValid(Actor))
				continue;

			if (!Filter(Actor))
				continue;

			const float DistSq = FVector::DistSquared(Location, Actor->GetActorLocation());
			if (DistSq < ClosestDistanceSq)
			{
				ClosestActor = Actor;
				ClosestDistanceSq = DistSq;
			}
		}

		return ClosestActor;
	}
};
