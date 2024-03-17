// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSType.h"
#include "Common/RSUtil.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSActorSpawnSubsystem.generated.h"

struct FRSActorSpawnUtil
{
	static void Activate(AActor* Actor);
	static void Deactivate(AActor* Actor);

	static void OnSpawn(AActor* Actor, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	static void OnDespawn(AActor* Actor);
};

UCLASS()
class PROJECTRS_API URSSpawnedActorPool : public UObject
{
	GENERATED_BODY()

public:
	
	void Init(UWorld* World, UClass* ActorClass, int32 InInitSize, int32 InMaxSize, int32 InGrowSize)
	{
		if (!World)
		{
			ensure(false);
			return;
		}

		if (!ActorClass)
		{
			ensure(false);
			return;
		}
		
		ensure(InInitSize <= InMaxSize);
		ensure(InInitSize > 0);
		ensure(InMaxSize > 0);
		ensure(InGrowSize > 0);
		
		InitSize = FMath::Max(1, InInitSize);
		MaxSize = FMath::Max(1, InMaxSize);
		GrowSize = FMath::Max(1, InGrowSize);
		SpawnSize = 0;
		LiveActorCount = 0;
		
		PoolArray.Reset();
		for (int32 Index = 0; Index < InitSize; ++Index)
		{
			PreSpawn(World, ActorClass);
		}
		
		UE_LOG(LogRS, Display, TEXT("%s Pool is Initialized(InitSize: %d, MaxSize: %d, GrowSize: %d)"),
			*ActorClass->GetName(), InitSize, MaxSize, GrowSize);
	}
	
	void Reset()
	{
		InitSize = 0;
		MaxSize = 0;
		GrowSize = 0;
		SpawnSize = 0;
		LiveActorCount = 0;
		
		PoolArray.Reset();
	}


	AActor* Spawn(UWorld* World, UClass* ActorClass, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		if (!World)
		{
			ensure(false);
			return nullptr;
		}

		if (!ActorClass)
		{
			ensure(false);
			return nullptr;
		}
		
		if (PoolArray.Num() == 0)
		{
			if (SpawnSize < MaxSize)
			{
				const int32 RemainSize = MaxSize - SpawnSize;
				const int32 CurGrowSize = FMath::Min(GrowSize, RemainSize);
				for (int32 Index = 0; Index < CurGrowSize; ++Index)
				{
					PreSpawn(World, ActorClass);
				}
				UE_LOG(LogRS, Display, TEXT("%s Pool Grow: %d, SpawnSize: %d"), *ActorClass->GetName(), CurGrowSize, SpawnSize);
			}
		}
		
		AActor* Actor;
		if (PoolArray.Num() > 0)
		{
			Actor = PoolArray.Pop();
			FRSActorSpawnUtil::Activate(Actor);
		}
		else
		{
			UE_LOG(LogRS, Warning, TEXT("%s Pool Exceed: Max(%d)"), *ActorClass->GetName(), MaxSize);
			Actor = URSUtil::SpawnActor<AActor>(World, ActorClass, SpawnLocation, SpawnRotation);
		}
		
		if (!IsValid(Actor))
		{
			ensure(false);
			return nullptr;
		}
		
		ensure(Actor->GetClass() == ActorClass);
		FRSActorSpawnUtil::OnSpawn(Actor, SpawnLocation, SpawnRotation);

		++LiveActorCount;
		UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Actor Count: %d"), *ActorClass->GetName(), LiveActorCount);

		return Actor;
	}

	void Despawn(AActor* Actor)
	{
		if (!IsValid(Actor))
		{
			ensure(false);
			return;
		}

		FRSActorSpawnUtil::OnDespawn(Actor);
		
		--LiveActorCount;
		UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Actor Count: %d"), *Actor->GetClass()->GetName(), LiveActorCount);
		
		if (LiveActorCount < MaxSize)
		{
			FRSActorSpawnUtil::Deactivate(Actor);
			PoolArray.Push(Actor);
		}
		else
		{
			Actor->Destroy();
		}
		
	}
	
private:
	void PreSpawn(UWorld* World, UClass* ActorClass)
	{
		AActor* Actor = URSUtil::SpawnActor<AActor>(World, ActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (!IsValid(Actor))
		{
			ensure(false);
			return;
		}
		
		FRSActorSpawnUtil::Deactivate(Actor);
		PoolArray.Push(Actor);
		++SpawnSize;
	}

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> PoolArray;

	int32 InitSize = 0;
	int32 MaxSize = 0;
	int32 GrowSize = 0;
	int32 SpawnSize = 0;
	int32 LiveActorCount = 0;
};

/**
 * 
 */
UCLASS()
class PROJECTRS_API URSActorSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static URSActorSpawnSubsystem* Get(const UWorld* World);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	bool IsInitialized() const { return bInitialized; }
	void SetDefaultSize(int32 InitSize, int32 MaxSize, int32 GrowSize);

	void InitPool(UClass* ActorClass, int32 InInitSize, int32 InMaxSize, int32 InGrowSize)
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			ensure(false);
			return;
		}
		
		if (!ActorClass)
		{
			ensure(false);
			return;
		}
		
		if (!SpawnedActorPoolMap.Contains(ActorClass))
		{
			SpawnedActorPoolMap.Add(ActorClass, NewObject<URSSpawnedActorPool>());
		}
		
		SpawnedActorPoolMap[ActorClass]->Init(World, ActorClass, InInitSize, InMaxSize, InGrowSize);
	}
	
	void ResetPool(const UClass* ActorClass)
	{
		if (!ActorClass)
		{
			ensure(false);
			return;
		}
		
		if (!SpawnedActorPoolMap.Contains(ActorClass))
		{
			ensure(false);
			return;
		}
		
		SpawnedActorPoolMap[ActorClass]->Reset();
	}

	AActor* Spawn(UClass* ActorClass, const FVector& SpawnLocation, const FRotator& SpawnRotation, bool bUsePool = true)
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			ensure(false);
			return nullptr;
		}
		
		if (!ActorClass)
		{
			ensure(false);
			return nullptr;
		}

		AActor* Actor;
		if (bUsePool)
		{
			if (!SpawnedActorPoolMap.Contains(ActorClass))
			{
				InitPool(ActorClass, DefaultInitSize, DefaultMaxSize, DefaultGrowSize);
			}
		
			Actor = SpawnedActorPoolMap[ActorClass]->Spawn(World, ActorClass, SpawnLocation, SpawnRotation);
		}
		else
		{
			Actor = URSUtil::SpawnActor<AActor>(World, ActorClass, SpawnLocation, SpawnRotation);
			if (IsValid(Actor))
			{
				FRSActorSpawnUtil::OnSpawn(Actor, SpawnLocation, SpawnRotation);
			}
		}

		return Actor;
	}

	void Despawn(AActor* Actor, bool bUsePool = true)
	{
		if (!IsValid(Actor))
			return;
		
		const UClass* ActorClass = Actor->GetClass();
		if (!ActorClass)
		{
			ensure(false);
			FRSActorSpawnUtil::OnDespawn(Actor);
			Actor->Destroy();
			return;
		}
		
		if (bUsePool)
		{
			if (!SpawnedActorPoolMap.Contains(ActorClass))
			{
				ensure(false);
				FRSActorSpawnUtil::OnDespawn(Actor);
				Actor->Destroy();
				return;
			}

			SpawnedActorPoolMap[ActorClass]->Despawn(Actor);
		}
		else
		{
			FRSActorSpawnUtil::OnDespawn(Actor);
			Actor->Destroy();
		}
	}
	
private:
	UPROPERTY(Transient)
	TMap<UClass*, TObjectPtr<URSSpawnedActorPool>> SpawnedActorPoolMap;

	int32 DefaultInitSize = 16;
	int32 DefaultMaxSize = 256;
	int32 DefaultGrowSize = 8;
	bool bInitialized = false;
};
