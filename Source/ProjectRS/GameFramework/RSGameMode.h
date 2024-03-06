// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RSGameMode.generated.h"

class ARSMonster;
class URSWidget_HUD;

/**
 * 
 */
UCLASS()
class PROJECTRS_API ARSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ARSGameMode();

protected:
	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnSpawnTimer();

private:
	bool CheckGameOver();
	void SpawnMonster();
	void UpdateMonster();
	void TeleportMonster();

	bool FindRandomSpawnLocation(FVector& OutSpawnLocation) const;
	bool IsSpawnableLocation(const FVector& SpawnLocation) const;

private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<ARSMonster>> SpawnedMonster;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FTimerHandle SpawnTimerHandle;
};
