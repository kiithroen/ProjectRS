// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/RSSkill.h"
#include "RSSkill_Dash.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class URSSkill_Dash : public URSSkill
{
	GENERATED_BODY()
	
public:
	virtual void OnBegin() override;
	virtual void OnEnd() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skill|Dash")
	TObjectPtr<UCurveVector> DashCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Dash")
	FVector DashCurveScale = FVector::OneVector;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Dash")
	bool bGhostMode = true;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Dash")
	TSoftObjectPtr<UAnimMontage> MontageToPlay;
};
