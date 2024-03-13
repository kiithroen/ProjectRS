// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RSProjectile.generated.h"

class URSProjectileMovementComponent;
class USphereComponent;
class UPaperFlipbookComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnProjectileHit, ARSProjectile*, const TArray<FHitResult>&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnProjectileExplosion, ARSProjectile*, int32);

UCLASS(Abstract)
class PROJECTRS_API ARSProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ARSProjectile();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TObjectPtr<AActor> Caster;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TObjectPtr<UObject> Trigger;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<AActor>> ActorsToIgnore;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	int32 HitCount = 0;
	
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	int32 ExplosionHitCount = 0;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FVector PrevLocation;
	
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FTimerHandle ExplosionTimerHandle;
};
