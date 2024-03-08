// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "RSType.h"
#include "RSAIController.generated.h"

class UAISenseConfig_Sight;
class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;

UCLASS(Abstract)
class PROJECTRS_API ARSAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ARSAIController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	AActor* GetTopAggro() const;
};
