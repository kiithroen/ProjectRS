// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RSBaseItem.h"

#include "RSUsableItem.generated.h"

class URSSkill;

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSUsableItem : public FRSBaseItem
{
	GENERATED_BODY()

public:
	FRSUsableItem()
	{
		Type = ERSItemType::Usable;
	}
	
	UPROPERTY(EditDefaultsOnly, Category = "UsableItem")
	TSubclassOf<URSSkill> SkillClass;
};