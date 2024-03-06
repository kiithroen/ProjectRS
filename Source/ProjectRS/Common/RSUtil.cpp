// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/RSUtil.h"
#include "HAL/FileManagerGeneric.h"
#include <random>

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

void URSUtil::SphereTraceMulti(const UObject* WorldContextObject, const FVector& Start, const FVector& End, float Radius, ECollisionChannel CollisionChannel, bool bTraceComplex, const AActor* IgnoreActor, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults)
{
	static const FName SphereTraceMultiName(TEXT("SphereTraceMulti"));
	FCollisionQueryParams Params(SphereTraceMultiName, bTraceComplex, IgnoreActor);
	Params.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	World ? World->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(Radius), Params) : false;
}


void URSUtil::BoxTraceMulti(const UObject* WorldContextObject, const FVector& Start, const FVector& End, const FVector& HalfSize, const FRotator& Orientation, ECollisionChannel CollisionChannel, bool bTraceComplex, const AActor* IgnoreActor, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHitResults)
{
	static const FName BoxTraceMultiName(TEXT("BoxTraceMulti"));
	FCollisionQueryParams Params(BoxTraceMultiName, bTraceComplex, IgnoreActor);
	Params.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	World ? World->SweepMultiByChannel(OutHitResults, Start, End, Orientation.Quaternion(), CollisionChannel, FCollisionShape::MakeBox(HalfSize), Params) : false;
}

void URSUtil::SphereOverlapMulti(const UObject* WorldContextObject, const FVector& Origin, float Radius, ECollisionChannel CollisionChannel, bool bTraceComplex, const AActor* IgnoreActor, const TArray<AActor*>& ActorsToIgnore, TArray<FOverlapResult>& OutOverlapResults)
{
	static const FName SphereOverlapMultiName(TEXT("SphereOverlapMulti"));
	FCollisionQueryParams Params(SphereOverlapMultiName, bTraceComplex, IgnoreActor);
	Params.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	World ? World->OverlapMultiByChannel(OutOverlapResults, Origin, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(Radius), Params) : false;
}

void URSUtil::BoxOverlapMulti(const UObject* WorldContextObject, const FVector& Origin, const FVector& HalfSize, const FRotator& Orientation, ECollisionChannel CollisionChannel, bool bTraceComplex, const AActor* IgnoreActor, const TArray<AActor*>& ActorsToIgnore, TArray<FOverlapResult>& OutOverlapResults)
{
	static const FName BoxOverlapMultiName(TEXT("BoxOverlapMulti"));
	FCollisionQueryParams Params(BoxOverlapMultiName, bTraceComplex, IgnoreActor);
	Params.AddIgnoredActors(ActorsToIgnore);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	World ? World->OverlapMultiByChannel(OutOverlapResults, Origin, Orientation.Quaternion(), CollisionChannel, FCollisionShape::MakeBox(HalfSize), Params) : false;
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