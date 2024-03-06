// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RSBaseItem.h"
#include "GameFramework/RSAssetManager.h"
#include "RSTokenItem.generated.h"


USTRUCT(BlueprintType)
struct PROJECTRS_API FRSTokenItem : public FRSBaseItem
{
	GENERATED_BODY()

public:
	FRSTokenItem()
	{
		Type = ERSItemType::Token;
	}
};
