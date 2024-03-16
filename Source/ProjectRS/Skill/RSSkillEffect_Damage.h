// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/RSSkillEffect.h"
#include "ScalableFloat.h"
#include "RSSkillEffect_Damage.generated.h"

class UCurveVector;

UCLASS()
class PROJECTRS_API URSSkillEffect_Damage : public URSSkillEffect
{
	GENERATED_BODY()
	
public:
	virtual URSSkillEffect* Clone() const override;
	virtual void Release() override;
	virtual void CopyFrom(const URSSkillEffect* Other) override;

	virtual void Do(float DeltaTime) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat BaseDamage = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat AttackDamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float RandomDamageMultiplier = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bIgnoreArmor = false;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bAlwaysCritical = false;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float LaunchSpeed = 500.f;
};
