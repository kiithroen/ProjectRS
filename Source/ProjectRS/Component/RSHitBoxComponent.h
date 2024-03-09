// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "RSHitBoxComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTRS_API URSHitBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	URSHitBoxComponent();
};
