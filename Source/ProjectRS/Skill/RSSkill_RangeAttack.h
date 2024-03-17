// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSSkill.h"
#include "ScalableFloat.h"
#include "RSSkill_RangeAttack.generated.h"

class ARSProjectile;
/**
 * 
 */
UCLASS(Abstract)
class PROJECTRS_API URSSkill_RangeAttack : public URSSkill
{
	GENERATED_BODY()

public:
	URSSkill_RangeAttack();

	virtual void OnBegin() override;
	virtual void Do(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnProjectileHit(ARSProjectile* Projectile, const TArray<FHitResult>& HitResults);

	UFUNCTION()
	void OnProjectileExplosion(ARSProjectile* Projectile, int32 HitCount);
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	int32 AmmoCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	float ShotInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	float AttackRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = TraceTypeQuery1;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	TSubclassOf<ARSProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	FName ProjectileSocketName = FName("ProjectileSocket");

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	float ProjectileSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	FScalableFloat ProjectileCount = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	bool bAutoAiming = true;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	float AimAngleRange = 90.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	float ExplosionLifeTime = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	int32 ExplosionHitCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|RangeAttack")
	float ExplosionSplashRange = 0.f;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Skill|RangeAttack")
	TArray<TObjectPtr<URSSkillEffect>> SkillEffectOnHit;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Skill|RangeAttack")
	TArray<TObjectPtr<URSSkillEffect>> SkillEffectOnExpolosion;

private:
	int32 RemainAmmoCount = 0;
	float RemainShotTime = 0.f;
};
