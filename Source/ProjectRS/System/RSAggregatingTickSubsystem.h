// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSAggregatingTickSubsystem.generated.h"

USTRUCT()
struct FRSComponentCollectionTickFucntion : public FTickFunction
{
	GENERATED_BODY()

	FRSComponentCollectionTickFucntion();
	
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
struct TStructOpsTypeTraits<FRSComponentCollectionTickFucntion> : public TStructOpsTypeTraitsBase2<FRSComponentCollectionTickFucntion>
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
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	bool IsInitialized() const { return bInitialized; }
	
	void RegisterComponent(UActorComponent* ActorComponent, ETickingGroup TickGroup);
	void UnRegisterComponent(UActorComponent* ActorComponent);

private:
	TMap<UClass*, TSharedPtr<FRSComponentCollectionTickFucntion>> ComponentCollectionTickFunctionMap;
	bool bInitialized = false;
};
