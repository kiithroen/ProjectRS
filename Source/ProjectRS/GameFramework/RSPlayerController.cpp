// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/RSCharacter.h"
#include "Skill/RSSkillComponent.h"

ARSPlayerController::ARSPlayerController()
{
}

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ARSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARSPlayerController::OnInputMove);
		EnhancedInputComponent->BindAction(ButtonAAction, ETriggerEvent::Triggered, this, &ARSPlayerController::OnInputButtonA);
	}
}

void ARSPlayerController::OnInputMove(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	MoveRight(MovementVector.X);
	MoveForward(MovementVector.Y);
}

void ARSPlayerController::MoveForward(float Value)
{
	APawn* OwnerPawn = GetPawn();
	if (!OwnerPawn)
		return;

	if (Value != 0.0f)
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		GetPawn()->AddMovementInput(Direction, Value);
	}
}

void ARSPlayerController::MoveRight(float Value)
{
	APawn* OwnerPawn = GetPawn();
	if (!OwnerPawn)
		return;

	if (Value != 0.0f)
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		GetPawn()->AddMovementInput(Direction, Value);
	}
}

void ARSPlayerController::OnInputButtonA(const FInputActionValue& Value)
{
	if (const ARSCharacter* PC = Cast<ARSCharacter>(GetCharacter()))
	{
		if (Value.Get<bool>())
		{
			if (URSSkillComponent* SkillComp = PC->FindComponentByClass<URSSkillComponent>())
			{
				SkillComp->SendEvent(RSGT_Skill_Event_Input_ButtonA);
			}
		}
	}
}
