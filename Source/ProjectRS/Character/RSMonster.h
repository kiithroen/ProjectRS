// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacter.h"
#include "Interface/RSSpawnInterface.h"
#include "RSMonster.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PROJECTRS_API ARSMonster : public ARSCharacter
{
	GENERATED_BODY()
	
public:
	static ARSMonster* Spawn(UWorld* World, URSCharacterPreset* Preset, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
	ARSMonster(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void OnSpawn() override;
	virtual void OnDespawn() override;
	
	virtual void ApplyDamage(float Damage, AActor* Caster) override;
	virtual void ApplyDie(AActor* Caster) override;
	
	virtual void LifeSpanExpired() override;
	
	UFUNCTION()
	void OnHitStopTimeout();
	
	UFUNCTION()
	void OnUpdateBuryCorpse();
	
private:
	void PlayHitStop();
	void ClearHitStopTimer();
	void ClearBuryCorpseTimer();

	void StartBuryCorpse();

	FTimerHandle HitStopTimerHandle;
	FTimerHandle BuryCorpseTimerHandle;

	FVector SavedMeshRelativeLocation;
};
