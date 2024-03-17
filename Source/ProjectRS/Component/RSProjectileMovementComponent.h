// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/RSActorPoolInterface.h"
#include "RSProjectileMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTRS_API URSProjectileMovementComponent : public UProjectileMovementComponent, public IRSActorPoolInterface
{
	GENERATED_BODY()

public:
	URSProjectileMovementComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void BeginPlayFromPool() override;
	virtual void EndPlayFromPool() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
