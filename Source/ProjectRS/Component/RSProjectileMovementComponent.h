// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/RSSpawnInterface.h"
#include "RSProjectileMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTRS_API URSProjectileMovementComponent : public UProjectileMovementComponent, public IRSSpawnInterface
{
	GENERATED_BODY()

public:
	URSProjectileMovementComponent();

	virtual void OnSpawn() override;
	virtual void OnDespawn() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
