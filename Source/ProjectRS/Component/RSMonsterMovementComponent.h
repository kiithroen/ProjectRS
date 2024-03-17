// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSCharacterMovementComponent.h"
#include "Interface/RSSpawnInterface.h"
#include "RSMonsterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTRS_API URSMonsterMovementComponent : public URSCharacterMovementComponent
{
	GENERATED_BODY()

public:
	URSMonsterMovementComponent();

	virtual void OnSpawn() override;
	virtual void OnDespawn() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void PerformMovement(float DeltaTime) override;
	virtual void PhysNavWalking(float DeltaTime, int32 Iterations) override;
};
