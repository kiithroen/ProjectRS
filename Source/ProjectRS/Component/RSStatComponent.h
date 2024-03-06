// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RSStatComponent.generated.h"

class URSStatComponent;

DECLARE_MULTICAST_DELEGATE_FourParams(FRSOnStatValueChanged, URSStatComponent*, const FGameplayTag&, float, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTRS_API URSStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URSStatComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Init(const TMap<FGameplayTag, float>& InDefaultStatValueMap);
	bool HasValue(const FGameplayTag& Tag) const;
	float GetValue(const FGameplayTag& Tag) const;
	void SetValue(const FGameplayTag& Tag, float Value);
	void AddValue(const FGameplayTag& Tag, float Value);

	FRSOnStatValueChanged OnStatValueChanged;

private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TMap<FGameplayTag, float> StatValues;

};
