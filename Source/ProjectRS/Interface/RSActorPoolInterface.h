// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSActorPoolInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URSActorPoolInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTRS_API IRSActorPoolInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlayFromPool() = 0;
	virtual void EndPlayFromPool() = 0;
};
