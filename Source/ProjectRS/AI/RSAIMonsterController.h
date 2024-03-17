// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/RSAIController.h"
#include "RSAIMonsterController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRS_API ARSAIMonsterController : public ARSAIController
{
	GENERATED_BODY()
	
public:
	ARSAIMonsterController();
	
	void StartAI();
	void StopAI();
	
protected:
	UFUNCTION()
	void OnUpdateAI();

	void ClearAITimer();
	
private:
	FTimerHandle AITimerHandle;
};