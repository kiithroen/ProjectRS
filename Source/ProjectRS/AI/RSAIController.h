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

	virtual void SetGenericTeamId(const FGenericTeamId& Id) override { TeamId = Id; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override { return RSTeamType::Resolve(GetPawn(), &Other); }

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

	AActor* GetTopAggro() const;
	void ChangeSight(float Dist, float LooseSight, float Angle);

private:
	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<AActor>> SightAggroActors;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<AActor>> DamageAggroActors;

protected:
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FGenericTeamId TeamId;
};
