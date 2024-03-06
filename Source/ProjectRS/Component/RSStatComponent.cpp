// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/RSStatComponent.h"

URSStatComponent::URSStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void URSStatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnStatValueChanged.Clear();
}

void URSStatComponent::Init(const TMap<FGameplayTag, float>& InDefaultStatValueMap)
{
	StatValues = InDefaultStatValueMap;

	for (auto& Pair : StatValues)
	{
		OnStatValueChanged.Broadcast(this, Pair.Key, Pair.Value, Pair.Value);
	}
}

bool URSStatComponent::HasValue(const FGameplayTag& Tag) const
{
	return StatValues.Contains(Tag);
}

float URSStatComponent::GetValue(const FGameplayTag& Tag) const
{
	if (!HasValue(Tag))
		return 0.f;

	const float Value = StatValues[Tag];
	return Value;
}

void URSStatComponent::SetValue(const FGameplayTag& Tag, float Value)
{
	if (!HasValue(Tag))
		return;

	const float OldValue = StatValues[Tag];

	float& NewValue = StatValues[Tag];
	NewValue = Value;

	OnStatValueChanged.Broadcast(this, Tag, OldValue, NewValue);
}

void URSStatComponent::AddValue(const FGameplayTag& Tag, float Value)
{
	if (!HasValue(Tag))
		return;

	const float OldValue = StatValues[Tag];

	float& NewValue = StatValues[Tag];
	NewValue += Value;

	OnStatValueChanged.Broadcast(this, Tag, OldValue, NewValue);
}