// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSSkill.h"
#include "ScalableFloat.h"
#include "RSSkill_MeleeAttack.generated.h"

class ARSProjectile;
/**
 * 
 */
UCLASS()
class PROJECTRS_API URSSkill_MeleeAttack : public URSSkill
{
	GENERATED_BODY()

public:
	URSSkill_MeleeAttack();

	virtual void OnBegin() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnEnd() override;
	virtual void OnEvent(const FGameplayTag& EventTag) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skill|MeleeAttack")
	FName StartSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|MeleeAttack")
	FName EndSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|MeleeAttack")
	float Radius = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill|MeleeAttack")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|MeleeAttack")
	TSoftObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Skill|MeleeAttack")
	TArray<TObjectPtr<URSSkillEffect>> SkillEffectOnHit;

private:
	bool bTraceMeleeAttack = false;
	TArray<AActor*> ActorsToIgnore;
};
