// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/RSUtil.h"
#include <random>

#include "RSType.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable<int32> CVarShowTargetTrace(
	TEXT("rs.ShowTargetTrace"),
	0,
	TEXT("0: Disable, 1: Enable"),
	ECVF_Default
);

static TAutoConsoleVariable<int32> CVarShowTargetTraceSeconds(
	TEXT("rs.ShowTargetTraceSeconds"),
	1 / 30,
	TEXT(""),
	ECVF_Default
);

static std::mt19937 mt_rand;

void URSUtil::MTInit()
{
	std::random_device rd;
	mt_rand.seed(rd());
}

int32 URSUtil::MTRandRange(int32 Min, int32 Max)
{
	std::uniform_int<> dist(Min, Max);
	return dist(mt_rand);
}

float URSUtil::MTFRandRange(float Min, float Max)
{
	std::uniform_real_distribution<float> dist(Min, Max);
	return dist(mt_rand);
}

int32 URSUtil::RandomWeightedIndex(const TArray<int32>& Table)
{
	if (Table.IsEmpty())
	{
		ensure(false);
		return INDEX_NONE;
	}

	int32 MaxWeight = 0;
	for (auto& Weight : Table)
	{
		MaxWeight += Weight;
	}


	int32 Target = MTRandRange(1, MaxWeight);

	int32 Index = 0;
	for (auto& Weight : Table)
	{
		if (Target <= Weight)
			return Index;

		Target -= Weight;
		++Index;
	}

	ensure(false);
	return INDEX_NONE;
}

FName URSUtil::RandomWeightedName(const TMap<FName, int32>& Table)
{
	if (Table.IsEmpty())
	{
		ensure(false);
		return NAME_None;
	}

	int32 MaxWeight = 0;
	for (auto& Item : Table)
	{
		MaxWeight += Item.Value;
	}

	int32 Target = MTRandRange(1, MaxWeight);
	for (auto& Item : Table)
	{
		if (Target <= Item.Value)
			return Item.Key;

		Target -= Item.Value;
	}

	ensure(false);
	return NAME_None;
}

void URSUtil::CollectTargets_SocketTrace(const AActor* Actor, const FRSTargetInfo_SocketTrace& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults)
{
	const UMeshComponent* MeshComp = Actor->FindComponentByClass<UMeshComponent>();
	
	const FVector SocketStartLocation = MeshComp ? MeshComp->GetSocketLocation(TargetInfo.StartSocket) : Actor->GetActorLocation();
	const FVector SocketEndLocation = MeshComp ? MeshComp->GetSocketLocation(TargetInfo.EndSocket) : Actor->GetActorLocation();
	const FVector WorldDirection = UKismetMathLibrary::GetDirectionUnitVector(SocketStartLocation, SocketEndLocation);
	const FVector StartLocation = SocketStartLocation + WorldDirection * TargetInfo.Radius;
	const FVector EndLocation = SocketEndLocation - WorldDirection * TargetInfo.Radius;

	const EDrawDebugTrace::Type DrawDebugTraceType = CVarShowTargetTrace.GetValueOnGameThread() == 1 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	UKismetSystemLibrary::SphereTraceMulti(Actor, StartLocation, EndLocation, TargetInfo.Radius, TargetInfo.TraceChannel, false, ActorsToIgnore, DrawDebugTraceType, OutHitResults, true, FLinearColor::Red, FLinearColor::Green, CVarShowTargetTraceSeconds.GetValueOnGameThread());
}

void URSUtil::CollectTargets_BoxArea(const AActor* Actor, const FRSTargetInfo_BoxArea& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults)
{
	const FVector WorldDirection = UKismetMathLibrary::GetDirectionUnitVector(TargetInfo.StartLocation, TargetInfo.EndLocation);
	const FVector StartLocation = TargetInfo.StartLocation + WorldDirection * TargetInfo.HalfSize.X;
	const FVector EndLocation = TargetInfo.EndLocation - WorldDirection * TargetInfo.HalfSize.X;

	const EDrawDebugTrace::Type DrawDebugTraceType = CVarShowTargetTrace.GetValueOnGameThread() == 1 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	UKismetSystemLibrary::BoxTraceMulti(Actor, StartLocation, EndLocation, TargetInfo.HalfSize, WorldDirection.Rotation(), TargetInfo.TraceChannel, false, ActorsToIgnore, DrawDebugTraceType, OutHitResults, true, FLinearColor::Red, FLinearColor::Green, CVarShowTargetTraceSeconds.GetValueOnGameThread());
}

void URSUtil::CollectTargets_SphereArea(const AActor* Actor, const FRSTargetInfo_SphereArea& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults)
{
	const FVector WorldDirection = UKismetMathLibrary::GetDirectionUnitVector(TargetInfo.StartLocation, TargetInfo.EndLocation);
	const FVector StartLocation = TargetInfo.StartLocation + WorldDirection * TargetInfo.Radius;
	const FVector EndLocation = TargetInfo.EndLocation - WorldDirection * TargetInfo.Radius;

	const EDrawDebugTrace::Type DrawDebugTraceType = CVarShowTargetTrace.GetValueOnGameThread() == 1 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	UKismetSystemLibrary::SphereTraceMulti(Actor, StartLocation, EndLocation, TargetInfo.Radius, TargetInfo.TraceChannel, false, ActorsToIgnore, DrawDebugTraceType, OutHitResults, true, FLinearColor::Red, FLinearColor::Green, CVarShowTargetTraceSeconds.GetValueOnGameThread());
}

void URSUtil::CollectTargets_CylinderArea(const AActor* Actor, const FRSTargetInfo_CylinderArea& TargetInfo, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults)
{
	const float Height = FVector::Distance(TargetInfo.StartLocation, TargetInfo.EndLocation);
	const FVector HalfSize = FVector(Height * 0.5f, TargetInfo.Radius, TargetInfo.Radius);

	const FVector WorldDirection = UKismetMathLibrary::GetDirectionUnitVector(TargetInfo.StartLocation, TargetInfo.EndLocation);
	const FVector StartLocation = TargetInfo.StartLocation + WorldDirection * HalfSize.X;
	const FVector EndLocation = TargetInfo.EndLocation - WorldDirection * HalfSize.X;

	const EDrawDebugTrace::Type DrawDebugTraceType = CVarShowTargetTrace.GetValueOnGameThread() == 1 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::BoxTraceMulti(Actor, StartLocation, EndLocation, HalfSize, WorldDirection.Rotation(), TargetInfo.TraceChannel, false, ActorsToIgnore, DrawDebugTraceType, HitResults, true, FLinearColor::Red, FLinearColor::Green, CVarShowTargetTraceSeconds.GetValueOnGameThread());

	const float RadiusSquared = FMath::Square(TargetInfo.Radius);

	OutHitResults = HitResults.FilterByPredicate([=](const FHitResult& HitResult) {
		AActor* TargetActor = HitResult.GetActor();
		if (!IsValid(TargetActor))
			return false;

		if (FVector::DistSquared2D(StartLocation, TargetActor->GetActorLocation()) > RadiusSquared)
			return false;

		return true;
		});
	
	if (CVarShowTargetTrace.GetValueOnGameThread() == 1)
	{
		UKismetSystemLibrary::DrawDebugCylinder(Actor, TargetInfo.StartLocation, TargetInfo.EndLocation, TargetInfo.Radius, 36, FLinearColor::White, 0.1f, 1.f);
	}
}

float URSUtil::FindAngle2D(const FVector& StartDir, const FVector& TargetDir)
{
	const FRotator StartRot = StartDir.Rotation();
	const FRotator TargetRot = TargetDir.Rotation();
	float Angle = TargetRot.Yaw - StartRot.Yaw;
	if (Angle > 180)
	{
		Angle = Angle - 360;
	}

	if (Angle < -180)
	{
		Angle = Angle + 360;
	}

	return Angle;
}