// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSHero.h"

#include "RSCharacterPreset.h"
#include "Common/RSUtil.h"
#include "Component/RSHeroMovementComponent.h"

ARSHero* ARSHero::Spawn(UWorld* World, URSCharacterPreset* Preset, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!World)
		return nullptr;

	if (!Preset)
	{
		ensure(false);
		return nullptr;
	}

	UClass* CharacterClass = Preset->CharacterClass.LoadSynchronous();
	if (!CharacterClass)
		return nullptr;

	ARSHero* Hero = URSUtil::SpawnActor<ARSHero>(World, CharacterClass, SpawnLocation, SpawnRotation);
	if (!IsValid(Hero))
		return nullptr;

	Hero->Init(Preset);

	return Hero;
}

ARSHero::ARSHero(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<URSHeroMovementComponent>(CharacterMovementComponentName))
{
}

void ARSHero::ApplyDamage(float Damage, AActor* Caster)
{
	Super::ApplyDamage(Damage, Caster);
}

void ARSHero::ApplyDie(AActor* Caster)
{
	Super::ApplyDie(Caster);
}