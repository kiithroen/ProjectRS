// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSCharacterMovementComponent.h"
#include "RSHeroMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTRS_API URSHeroMovementComponent : public URSCharacterMovementComponent
{
	GENERATED_BODY()

public:
	URSHeroMovementComponent();
	
	virtual void OnSpawn() override;
	virtual void OnDespawn() override;
	
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
