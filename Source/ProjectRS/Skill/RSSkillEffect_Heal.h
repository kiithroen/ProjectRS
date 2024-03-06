// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/RSSkillEffect.h"
#include "ScalableFloat.h"
#include "RSSkillEffect_Heal.generated.h"

UCLASS()
class PROJECTRS_API URSSkillEffect_Heal : public URSSkillEffect
{
	GENERATED_BODY()
	
public:
	URSSkillEffect_Heal();

	virtual void Do(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	FScalableFloat BaseHeal = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	float RandomHealMultiplier = 0.1f;
};
