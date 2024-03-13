// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RSCharacterPreset.generated.h"

struct FRSSkillItem;
class ARSCharacter;

UCLASS()
class PROJECTRS_API URSCharacterPreset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "RS", meta = (Categories = "Character.Id"))
	FGameplayTag Id;

	UPROPERTY(EditDefaultsOnly, Category = "RS")
	TSoftClassPtr<ARSCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "RS", meta = (Categories = "Stat", ForceInlineRow))
	TMap<FGameplayTag, float> DefaultStatValues;

	UPROPERTY(EditDefaultsOnly, Category = "RS", meta = (Categories = "Skill.Id"))
	TArray<FGameplayTag> DefaultSkillItemIds;

	UPROPERTY(EditDefaultsOnly, Category = "RS", meta = (Categories = "Skill", ForceInlineRow))
	TMap<FGameplayTag, FGameplayTag> SlotToSkillId;
	
	UPROPERTY(EditDefaultsOnly, Category = "RS")
	TSoftObjectPtr<UAnimMontage> HitMontage;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};