// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "InputActionValue.h"
#include "RSType.h"
#include "RSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class PROJECTRS_API ARSPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ARSPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void OnInputMove(const FInputActionValue& Value);
	void OnInputButtonAPressed(const FInputActionValue& Value);

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	
protected:
	UPROPERTY(EditAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ButtonAAction;
};
