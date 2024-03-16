// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RSBaseItem.h"
#include "RSEquipItem.generated.h"


class URSSkill;

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSEquipItem : public FRSBaseItem
{
	GENERATED_BODY()

public:
	FRSEquipItem()
	{
		Type = ERSItemType::EquipItem;
	}
	
	UPROPERTY(EditDefaultsOnly, Category = "EquipItem")
	TSubclassOf<URSSkill> SkillClass;
};
