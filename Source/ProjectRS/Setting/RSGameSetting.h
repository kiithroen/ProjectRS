// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RSGameSetting.generated.h"

class URSGlobalData;

/**
 * 
 */
UCLASS(config = Game, DefaultConfig)
class PROJECTRS_API URSGameSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const URSGameSetting* Get();

	UPROPERTY(Config, EditDefaultsOnly, Category = "RS")
	TSoftObjectPtr<URSGlobalData> GlobalDataPath;
};
