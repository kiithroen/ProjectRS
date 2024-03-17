// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSType.h"
#include "Common/RSUtil.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSActorPoolSubsystem.generated.h"


UCLASS()
class PROJECTRS_API URSSpawnedActorPool : public UObject
{
	GENERATED_BODY()

public:
	static void Activate(AActor* Actor);
	static void Deactivate(AActor* Actor);

	static void BeginPlay(AActor* Actor, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	static void EndPlay(AActor* Actor);
	
	void InitPool(UWorld* World, UClass* ActorClass, int32 InInitSize, int32 InMaxSize, int32 InGrowSize)
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
			++LiveActorCount;
			UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Actor Count: %d"), *ActorClass->GetName(), LiveActorCount);

			Actor = PoolArray.Pop();
			Activate(Actor);
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
		BeginPlay(Actor, SpawnLocation, SpawnRotation);

		return Actor;
	}

	void Despawn(AActor* Actor)
	{
		if (!IsValid(Actor))
		{
			ensure(false);
			return;
		}

		EndPlay(Actor);
		
		if (PoolArray.Num() < MaxSize)
		{
			--LiveActorCount;
			UE_LOG(LogRS, Verbose, TEXT("%s Pool Live Actor Count: %d"), *Actor->GetClass()->GetName(), LiveActorCount);
			
			Deactivate(Actor);
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
		
		Deactivate(Actor);
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
class PROJECTRS_API URSActorPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static URSActorPoolSubsystem* Get(const UWorld* World);
	
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
		
		SpawnedActorPoolMap[ActorClass]->InitPool(World, ActorClass, InInitSize, InMaxSize, InGrowSize);
	}
	
	void Reset(const UClass* ActorClass)
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

	AActor* Spawn(UClass* ActorClass, const FVector& SpawnLocation, const FRotator& SpawnRotation)
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
		
		if (!SpawnedActorPoolMap.Contains(ActorClass))
		{
			InitPool(ActorClass, DefaultInitSize, DefaultMaxSize, DefaultGrowSize);
		}
		
		return SpawnedActorPoolMap[ActorClass]->Spawn(World, ActorClass, SpawnLocation, SpawnRotation);
	}

	void Despawn(AActor* Actor)
	{
		if (!IsValid(Actor))
			return;
		
		const UClass* ActorClass = Actor->GetClass();
		if (!ActorClass)
		{
			ensure(false);
			Actor->Destroy();
			return;
		}
		
		if (!SpawnedActorPoolMap.Contains(ActorClass))
		{
			ensure(false);
			Actor->Destroy();
			return;
		}
		
		SpawnedActorPoolMap[ActorClass]->Despawn(Actor);
	}
	
private:
	UPROPERTY(Transient)
	TMap<UClass*, TObjectPtr<URSSpawnedActorPool>> SpawnedActorPoolMap;

	int32 DefaultInitSize = 16;
	int32 DefaultMaxSize = 128;
	int32 DefaultGrowSize = 8;
	bool bInitialized = false;
};
