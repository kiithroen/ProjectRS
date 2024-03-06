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
	
	virtual void SetGenericTeamId(const FGenericTeamId& Id) override { TeamId = Id; }
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override { return RSTeamType::Resolve(GetPawn(), &Other); }

	void OnInputMove(const FInputActionValue& Value);
	void OnInputButtonA(const FInputActionValue& Value);

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

private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FGenericTeamId TeamId;
};
