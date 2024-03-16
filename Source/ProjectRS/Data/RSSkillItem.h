// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RSBaseItem.h"

#include "RSSkillItem.generated.h"

class URSSkill;

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSSkillItem : public FRSBaseItem
{
	GENERATED_BODY()

public:
	FRSSkillItem()
	{
		Type = ERSItemType::Skill;
	}
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	int32 InitialLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<URSSkill> SkillClass;
};
