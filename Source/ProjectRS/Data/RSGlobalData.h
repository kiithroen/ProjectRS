// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RSGlobalData.generated.h"

class URSCharacterPreset;
class UDataTable;
class ARSFieldItem;

/**
 * 
 */
UCLASS()
class PROJECTRS_API URSGlobalData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "RS")
	TArray<TObjectPtr<UDataTable>> DataTables;
	
	UPROPERTY(EditDefaultsOnly, Category = "RS")
	TObjectPtr<URSCharacterPreset> DefaultHero;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	TObjectPtr<URSCharacterPreset> DefaultMonster;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	int32 InitialMonsterNum = 0;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	int32 MaxMonsterNum = 300;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	float MonsterSpawnInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	float MonsterSpawnRangeMin = 700.f;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	float MonsterSpawnRangeMax = 1300.f;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	float MonsterTeleportRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	TArray<TSubclassOf<ARSFieldItem>> FieldItems;
};
