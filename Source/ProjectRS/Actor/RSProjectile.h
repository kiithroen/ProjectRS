// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/RSSpawnInterface.h"
#include "RSProjectile.generated.h"

class URSProjectileMovementComponent;
class USphereComponent;
class UPaperFlipbookComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnProjectileHit, ARSProjectile*, const TArray<FHitResult>&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnProjectileExplosion, ARSProjectile*, int32);

UCLASS(Abstract)
class PROJECTRS_API ARSProjectile : public AActor, public IRSSpawnInterface
{
	GENERATED_BODY()
	
public:	
	ARSProjectile();

	virtual void Tick(float DeltaTime) override;

	virtual void OnSpawn() override;
	virtual void OnDespawn() override;
	
	void SetCaster(AActor* Caster);
	AActor* GetCaster() const;
	void AddIgnoreActor(AActor* Actor);
	void SetTraceChannel(TEnumAsByte<ETraceTypeQuery> InTraceChannel);
	void SetSpeed(float InitialSpeed, float MaxSpeed);
	void SetHoming(const AActor* Target, float AccelMagnitude);
	void SetDirection(const FVector& Direction);
	void SetExplosion(float LifeTime, int32 ExplosionHitCount);

protected:
	void OnExplosion();

private:
	void ClearExplosionTimer();

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSProjectileMovementComponent> ProjectileMovementComp;

public:
	FRSOnProjectileHit OnProjectileHit;
	FRSOnProjectileExplosion OnProjectileExplosion;

private:
	UPROPERTY(Transient)
	TArray<AActor*> ActorsToIgnore;
	
	TWeakObjectPtr<AActor> Caster;
	TEnumAsByte<ETraceTypeQuery> TraceChannel = TraceTypeQuery1;
	int32 HitCount = 0;
	int32 ExplosionHitCount = 0;
	FVector PrevLocation = FVector::ZeroVector;
	FTimerHandle ExplosionTimerHandle;
};
