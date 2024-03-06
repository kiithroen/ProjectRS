// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "RSGameInstance.generated.h"

class URSDataTableManager;
class ARSGameMode;
class ARSPlayerController;
class ARSHero;

/**
 * 
 */
UCLASS()
class PROJECTRS_API URSGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
