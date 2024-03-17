// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RSCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTRS_API URSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	URSCharacterMovementComponent();
	
	FVector GetLastMovementDirection() const { return LastMovementDirection; }
	
	void StopMovementAll();
	float StartMovementByCurve(UCurveVector* Curve, const FVector& Direction, const FVector& Scale);
	void StopMovementByCurve();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void PerformMovement(float DeltaTime) override;
	
	void PerformMovementByCurve(float DeltaTime);
	void UpdateMovementByCurve(float DeltaTime);
	void UpdateLastMovementDirection();

protected:
	UPROPERTY(Transient)
	TObjectPtr<UCurveVector> MovementCurve;
	
	bool bMovementByCurve = false;
	float MovementByCurveElapsedTime = 0.f;
	FVector MovementCurveScale = FVector::OneVector;
	FVector MovementCurveDirection = FVector::ForwardVector;
	FVector LastMovementDirection = FVector::ZeroVector;
};
