// Fill out your copyright notice in the Description page of Project Settings.


#include "RSFieldItem.h"
#include "Character/RSHero.h"
#include "Skill/RSSkillComponent.h"
#include "Kismet/GameplayStatics.h"

ARSFieldItem::ARSFieldItem()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (MeshComponent)
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
		MeshComponent->SetGenerateOverlapEvents(false);
		MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		RootComponent = MeshComponent;
	}
}

void ARSFieldItem::BeginPlay()
{
	Super::BeginPlay();
}

void ARSFieldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ARSHero* Hero = Cast<ARSHero>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Hero))
		return;

	const FVector HeroLocation = Hero->GetActorLocation();
	const FVector ItemLocation = GetActorLocation();
	const FVector Direction = HeroLocation - ItemLocation;
	const float DistanceToHero = Direction.Length();
	if (DistanceToHero > MagneticDistance)
		return;

	if (DistanceToHero < 10.f)
	{
		OnHit(Hero);
	}
	else
	{
		float MoveDistance = 700.f * DeltaTime;
		if (MoveDistance > DistanceToHero)
			MoveDistance = DistanceToHero;

		const FVector MoveLocation = ItemLocation + (Direction.GetSafeNormal() * MoveDistance);
		SetActorLocation(MoveLocation);
	}
}

void ARSFieldItem::OnHit(AActor* Actor)
{
	if (!IsValid(Actor))
		return;

	URSSkillComponent* SkillComp = Actor->FindComponentByClass<URSSkillComponent>();
	if (!SkillComp)
		return;

	for (auto& SkillEffect : SkillEffectOnHit)
	{
		SkillComp->AddSkillEffect(SkillEffect, SkillEffectLevel, Actor);
	}

	Destroy();
}