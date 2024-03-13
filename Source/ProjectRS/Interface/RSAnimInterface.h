// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSAnimInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URSAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTRS_API IRSAnimInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float PlayMontage(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) = 0;
	virtual float PlayMontageWithEnd(UAnimMontage* AnimMontage, TFunction<void(UAnimMontage*,bool)> EndCallback, float InPlayRate = 1.f, FName StartSectionName = NAME_None) = 0;
};
