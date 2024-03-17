// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSAggregatingTickSubsystem.generated.h"

USTRUCT()
struct FRSActorCollectionTickFunction : public FTickFunction
{
	GENERATED_BODY()

	FRSActorCollectionTickFunction() {}
	
	void AddActor(AActor* AActor)
	{
		PendingTickingActors.Emplace(AActor);
	}

	void RemoveActor(AActor* AActor)
	{
		ExpiredTickingActors.Emplace(AActor);
	}
	
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)	override;
	
	TArray<TWeakObjectPtr<AActor>> TickingActors;
	TArray<TWeakObjectPtr<AActor>> PendingTickingActors;
	TArray<TWeakObjectPtr<AActor>> ExpiredTickingActors;
};

template<>
struct TStructOpsTypeTraits<FRSActorCollectionTickFunction> : public TStructOpsTypeTraitsBase2<FRSActorCollectionTickFunction>
{
	enum
	{
		WithCopy = false
	};
};


USTRUCT()
struct FRSComponentCollectionTickFunction : public FTickFunction
{
	GENERATED_BODY()

	FRSComponentCollectionTickFunction() {}
	
	void AddComponent(UActorComponent* ActorComponent)
	{
		PendingTickingComponents.Emplace(ActorComponent);
	}

	void RemoveComponent(UActorComponent* ActorComponent)
	{
		ExpiredTickingComponents.Emplace(ActorComponent);
	}
	
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)	override;
	
	TArray<TWeakObjectPtr<UActorComponent>> TickingComponents;
	TArray<TWeakObjectPtr<UActorComponent>> PendingTickingComponents;
	TArray<TWeakObjectPtr<UActorComponent>> ExpiredTickingComponents;
};

template<>
struct TStructOpsTypeTraits<FRSComponentCollectionTickFunction> : public TStructOpsTypeTraitsBase2<FRSComponentCollectionTickFunction>
{
	enum
	{
		WithCopy = false
	};
};

/**
 * 
 */
UCLASS()
class PROJECTRS_API URSAggregatingTickSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static URSAggregatingTickSubsystem* Get(const UWorld* World);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	bool IsInitialized() const { return bInitialized; }

	void RegisterActor(AActor* Actor, ETickingGroup TickGroup);
	void UnRegisterActor(AActor* Actor);
	
	void RegisterComponent(UActorComponent* ActorComponent, ETickingGroup TickGroup);
	void UnRegisterComponent(UActorComponent* ActorComponent);

private:
	TMap<UClass*, TSharedPtr<FRSActorCollectionTickFunction>> ActorCollectionTickFunctionMap;
	TMap<UClass*, TSharedPtr<FRSComponentCollectionTickFunction>> ComponentCollectionTickFunctionMap;
	bool bInitialized = false;
};
