// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacter.h"
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
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	virtual void ApplyDamage(float Damage, AActor* Caster) override;
	virtual void ApplyDie(AActor* Caster) override;

	UFUNCTION()
	void OnHitStopTimeout();
	
private:
	void PlayHitStop();
	void ClearHitStopTimer();

	void StartBuryCoprse();
	void UpdateBuryCoprse(float DeltaTime);
	
	FTimerHandle HitStopTimerHandle;
	FVector DeathLocation;
	bool bBuryCorpse = false;
};
