// Fill out your copyright notice in the Description page of Project Settings.


#include "RSAggregatingTickSubsystem.h"

FRSComponentCollectionTickFucntion::FRSComponentCollectionTickFucntion()
{
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
