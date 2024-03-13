// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Styling/SlateBrush.h"
#include "RSBaseItem.generated.h"

UENUM(BlueprintType)
enum class ERSItemType : uint8
{
	None,
	Usable,
	EquipItem,
	Token,
	Skill,
};

USTRUCT(BlueprintType)
struct PROJECTRS_API FRSBaseItem : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FGameplayTag Id;
	
	UPROPERTY(VisibleAnywhere, Category = "RS")
	ERSItemType Type;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FText Name;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FText Description;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	FSlateBrush Icon;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	int32 Price = 0;
};