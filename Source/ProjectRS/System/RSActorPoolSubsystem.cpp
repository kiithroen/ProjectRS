// Fill out your copyright notice in the Description page of Project Settings.


#include "RSActorPoolSubsystem.h"
#include "Interface/RSActorPoolInterface.h"

void URSSpawnedActorPool::Activate(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		ensure(false);
		return;
	}
	
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);
	
	TInlineComponentArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (Component)
		{
			Component->Activate();
		}
	}
}

void URSSpawnedActorPool::Deactivate(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		ensure(false);
		return;
	}
	
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
			
	TInlineComponentArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (Component)
		{
			Component->Deactivate();
		}
	}
}

void URSSpawnedActorPool::BeginPlay(AActor* Actor, const FVector& Location, const FRotator& Rotation)
{
	if (!IsValid(Actor))
	{
		ensure(false);
		return;
	}
	
	Actor->SetActorLocationAndRotation(Location, Rotation);
	if (IRSActorPoolInterface* ActorPoolInterface = Cast<IRSActorPoolInterface>(Actor))
	{
		ActorPoolInterface->BeginPlayFromPool();
	}
	
	TInlineComponentArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (IRSActorPoolInterface* ActorPoolInterface = Cast<IRSActorPoolInterface>(Component))
		{
			ActorPoolInterface->BeginPlayFromPool();
		}
	}
}

void URSSpawnedActorPool::EndPlay(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		ensure(false);
		return;
	}
	
	if (IRSActorPoolInterface* ActorPoolInterface = Cast<IRSActorPoolInterface>(Actor))
	{
		ActorPoolInterface->EndPlayFromPool();
	}
			
	TInlineComponentArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (IRSActorPoolInterface* ActorPoolInterface = Cast<IRSActorPoolInterface>(Component))
		{
			ActorPoolInterface->EndPlayFromPool();
		}
	}
}

URSActorPoolSubsystem* URSActorPoolSubsystem::Get(const UWorld* World)
{
	if (!World)
		return nullptr;

	return World->GetSubsystem<URSActorPoolSubsystem>();
}

void URSActorPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;
}

void URSActorPoolSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}

void URSActorPoolSubsystem::SetDefaultSize(int32 InitSize, int32 MaxSize, int32 GrowSize)
{
	DefaultInitSize = InitSize;
	DefaultMaxSize = MaxSize;
	DefaultGrowSize = GrowSize;
}
