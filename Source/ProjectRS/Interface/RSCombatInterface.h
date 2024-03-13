// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "RSCombatInterface.generated.h"


class UCurveVector;

// This class does not need to be modified.
UINTERFACE()
class URSCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTRS_API IRSCombatInterface
{
	GENERATED_BODY()

public:
	virtual bool IsDead() const = 0;
	virtual bool IsImmune() const = 0;
	virtual void ApplyHeal(float Damage, AActor* InCaster) = 0;
	virtual void ApplyDamage(float Damage, AActor* InCaster) = 0;
	virtual void ApplyDie(AActor* InCaster) = 0;
	
	virtual void Launch(const FVector& Velocity, AActor* Caster) = 0;
	virtual void StopMovementAll() = 0;
	virtual void StartMovementByCurve(UCurveVector* Curve, const FVector& Direction, const FVector& Scale) = 0;
	virtual void StopMovementByCurve() = 0;
	virtual void EnableMovementInput(bool bEnable) = 0;
	virtual FVector GetLastMovementDirection() const = 0;
	
	virtual void EnableGhost(bool bEnable) = 0;

	virtual bool UseSkillSlot(const FGameplayTag& Slot) = 0;
	
	virtual float GetStat(const FGameplayTag& Tag) const = 0;
	virtual void SetStat(const FGameplayTag& Tag, float Value) const = 0;
	virtual void AddStat(const FGameplayTag& Tag, float Value) const = 0;
};
