// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/RSCharacterPreset.h"
#include "GameFramework/RSAssetManager.h"

FPrimaryAssetId URSCharacterPreset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(URSAssetManager::CharacterPreset, GetFName());
}