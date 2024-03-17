// Fill out your copyright notice in the Description page of Project Settings.


#include "RSActorSpawnSubsystem.h"
#include "Interface/RSSpawnInterface.h"

void FRSActorSpawnUtil::Activate(AActor* Actor)
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

void FRSActorSpawnUtil::Deactivate(AActor* Actor)
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

void FRSActorSpawnUtil::OnSpawn(AActor* Actor, const FVector& Location, const FRotator& Rotation)
{
	if (!IsValid(Actor))
	{
		ensure(false);
		return;
	}
	
	Actor->SetActorLocationAndRotation(Location, Rotation);
	if (IRSSpawnInterface* ActorPoolInterface = Cast<IRSSpawnInterface>(Actor))
	{
		ActorPoolInterface->OnSpawn();
	}
	
	TInlineComponentArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (IRSSpawnInterface* ActorPoolInterface = Cast<IRSSpawnInterface>(Component))
		{
			ActorPoolInterface->OnSpawn();
		}
	}
}

void FRSActorSpawnUtil::OnDespawn(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		ensure(false);
		return;
	}
	
	if (IRSSpawnInterface* ActorPoolInterface = Cast<IRSSpawnInterface>(Actor))
	{
		ActorPoolInterface->OnDespawn();
	}
			
	TInlineComponentArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (IRSSpawnInterface* ActorPoolInterface = Cast<IRSSpawnInterface>(Component))
		{
			ActorPoolInterface->OnDespawn();
		}
	}
}

URSActorSpawnSubsystem* URSActorSpawnSubsystem::Get(const UWorld* World)
{
	if (!World)
		return nullptr;

	return World->GetSubsystem<URSActorSpawnSubsystem>();
}

void URSActorSpawnSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;
}

void URSActorSpawnSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}

void URSActorSpawnSubsystem::SetDefaultSize(int32 InitSize, int32 MaxSize, int32 GrowSize)
{
	DefaultInitSize = InitSize;
	DefaultMaxSize = MaxSize;
	DefaultGrowSize = GrowSize;
}
