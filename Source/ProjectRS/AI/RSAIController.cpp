// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/RSAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"

ARSAIController::ARSAIController()
{
	TeamId = RSTeamType::Neutral;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 10000;
		SightConfig->LoseSightRadius = 10000;
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 0;
		SightConfig->PeripheralVisionAngleDegrees = 180;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
		SightConfig->SetMaxAge(0);
	}

	if (DamageConfig)
	{
		DamageConfig->SetMaxAge(0);
	}

	if (AIPerception)
	{
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->ConfigureSense(*DamageConfig);

		AIPerception->SetDominantSense(DamageConfig->GetSenseImplementation());
	}
}

void ARSAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ARSAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DamageAggroActors.RemoveAllSwap([](const AActor* Actor) {
			return !IsValid(Actor);
		});

	SightAggroActors.RemoveAllSwap([](const AActor* Actor) {
			return !IsValid(Actor);
		});
}

void ARSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AIPerception)
	{
		AIPerception->RequestStimuliListenerUpdate();
	}

	SetActorTickEnabled(true);
}

void ARSAIController::OnUnPossess()
{
	Super::OnUnPossess();

	SetActorTickEnabled(false);
}

void ARSAIController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (!AIPerception)
		return;

	for (auto& Actor : UpdatedActors)
	{
		if (!IsValid(Actor))
			continue;

		const FActorPerceptionInfo* Info = AIPerception->GetActorInfo(*Actor);
		if (!Info)
			continue;

		for (auto& Stimulus : Info->LastSensedStimuli)
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				// 들어옴
				if (Stimulus.Type == SightConfig->GetSenseID())
				{
					SightAggroActors.AddUnique(Actor);
				}
				else if (Stimulus.Type == DamageConfig->GetSenseID())
				{
					DamageAggroActors.AddUnique(Actor);
				}
				else
				{
					// nothing to do
				}
			}
			else
			{
				// 나감
				if (Stimulus.Type == SightConfig->GetSenseID())
				{
					SightAggroActors.Remove(Actor);
				}
				else if (Stimulus.Type == DamageConfig->GetSenseID())
				{
					DamageAggroActors.Remove(Actor);
				}
				else
				{
					// nothing to do
				}
			}
		}
		
	}
}

AActor* ARSAIController::GetTopAggro() const
{
	// 지금은 테스트 삼아 가장 첫번째 액터
	for (auto& Actor : DamageAggroActors)
	{
		return Actor;
	}

	for (auto& Actor : SightAggroActors)
	{
		return Actor;
	}

	return nullptr;
}

void ARSAIController::ChangeSight(float Sight, float LooseSight, float Angle)
{
	SightConfig->SightRadius = Sight;
	SightConfig->LoseSightRadius = LooseSight;
	SightConfig->PeripheralVisionAngleDegrees = Angle;

	if (AIPerception)
	{
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->RequestStimuliListenerUpdate();
	}
}