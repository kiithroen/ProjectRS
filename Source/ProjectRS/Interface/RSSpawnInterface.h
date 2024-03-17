// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSSpawnInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URSSpawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTRS_API IRSSpawnInterface
{
	GENERATED_BODY()

public:
	virtual void OnSpawn() = 0;
	virtual void OnDespawn() = 0;;
};
