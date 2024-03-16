// Fill out your copyright notice in the Description page of Project Settings.


#include "RSAggregatingTickSubsystem.h"


void FRSActorCollectionTickFucntion::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	for (auto& Actor : ExpiredTickingActors)
	{
		TickingActors.RemoveSingleSwap(Actor);
	}
	ExpiredTickingActors.Reset();

	TickingActors.Append(PendingTickingActors);
	PendingTickingActors.Reset();

	TickingActors.RemoveAllSwap([](const TWeakObjectPtr<AActor>& Actor)
	{
		return !Actor.IsValid();
	});

	for (auto& Actor : TickingActors)
	{
		Actor->Tick(DeltaTime);
	}
}

void FRSComponentCollectionTickFucntion::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	for (auto& Component : ExpiredTickingComponents)
	{
		TickingComponents.RemoveSingleSwap(Component);
	}
	ExpiredTickingComponents.Reset();

	TickingComponents.Append(PendingTickingComponents);
	PendingTickingComponents.Reset();

	TickingComponents.RemoveAllSwap([](const TWeakObjectPtr<UActorComponent>& Component)
	{
		return !Component.IsValid();
	});

	for (auto& Component : TickingComponents)
	{
		Component->TickComponent(DeltaTime, TickType, nullptr);
	}
}

void URSAggregatingTickSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;
}

void URSAggregatingTickSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}

void URSAggregatingTickSubsystem::RegisterActor(AActor* Actor, ETickingGroup TickGroup)
{
	UClass* ActorClass = Actor->GetClass();
	if (!ActorClass)
	{
		ensure(false);
		return;
	}
	
	if (!ActorCollectionTickFunctionMap.Contains(ActorClass))
	{
		TSharedPtr<FRSActorCollectionTickFucntion> ActorCollectionTickFucntion = MakeShareable(new FRSActorCollectionTickFucntion);
		ActorCollectionTickFucntion->TickGroup = TickGroup;
		
		ActorCollectionTickFucntion->RegisterTickFunction(Actor->GetLevel());
		ActorCollectionTickFunctionMap.Add(ActorClass, ActorCollectionTickFucntion);
	}
	
	ActorCollectionTickFunctionMap[ActorClass]->AddActor(Actor);
}

void URSAggregatingTickSubsystem::UnRegisterActor(AActor* Actor)
{
	UClass* ActorClass = Actor->GetClass();
	if (!ActorClass)
	{
		ensure(false);
		return;
	}
	
	if (!ActorCollectionTickFunctionMap.Contains(ActorClass))
	{
		ensure(false);
		return;
	}
	
	ActorCollectionTickFunctionMap[ActorClass]->RemoveActor(Actor);
}

void URSAggregatingTickSubsystem::RegisterComponent(UActorComponent* ActorComponent, ETickingGroup TickGroup)
{
	UClass* ActorComponentClass = ActorComponent->GetClass();
	if (!ActorComponentClass)
	{
		ensure(false);
		return;
	}
	
	if (!ComponentCollectionTickFunctionMap.Contains(ActorComponentClass))
	{
		TSharedPtr<FRSComponentCollectionTickFucntion> ComponentCollectionTickFucntion = MakeShareable(new FRSComponentCollectionTickFucntion);
		ComponentCollectionTickFucntion->TickGroup = TickGroup;
		ComponentCollectionTickFucntion->RegisterTickFunction(ActorComponent->GetComponentLevel());
		ComponentCollectionTickFunctionMap.Add(ActorComponentClass, ComponentCollectionTickFucntion);
	}
	
	ComponentCollectionTickFunctionMap[ActorComponentClass]->AddComponent(ActorComponent);
}

void URSAggregatingTickSubsystem::UnRegisterComponent(UActorComponent* ActorComponent)
{
	UClass* ActorComponentClass = ActorComponent->GetClass();
	if (!ActorComponentClass)
	{
		ensure(false);
		return;
	}
	
	if (!ComponentCollectionTickFunctionMap.Contains(ActorComponentClass))
	{
		ensure(false);
		return;
	}
	
	ComponentCollectionTickFunctionMap[ActorComponentClass]->RemoveComponent(ActorComponent);
}
