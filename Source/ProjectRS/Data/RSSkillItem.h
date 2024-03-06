// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RSBaseItem.h"
#include "GameFramework/RSAssetManager.h"

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill", meta = (Categories = "Skill.Id"))
	int32 InitialLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<URSSkill> SkillClass;
};
