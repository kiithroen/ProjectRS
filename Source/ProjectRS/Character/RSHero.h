// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacter.h"
#include "RSHero.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PROJECTRS_API ARSHero : public ARSCharacter
{
	GENERATED_BODY()
	
public:
	static ARSHero* Spawn(UWorld* World, URSCharacterPreset* Preset, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
	ARSHero(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void ApplyDamage(float Damage, AActor* Caster) override;
	virtual void ApplyDie(AActor* Caster) override;
};
