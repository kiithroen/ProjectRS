// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSMonster.h"

#include "RSCharacterPreset.h"
#include "GameFramework/RSAssetManager.h"
#include "Common/RsUtil.h"
#include "Actor/RSFieldItem.h"
#include "AI/RSAIMonsterController.h"
#include "Character/RSHero.h"
#include "Data/RSGlobalData.h"

ARSMonster* ARSMonster::Spawn(UWorld* World, URSCharacterPreset* Preset, const FVector& SpawnLocation, const FRotator& SpawnRotation)
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
	{
		ensure(false);
		return nullptr;
	}
	ARSMonster* Monster = URSUtil::SpawnActor<ARSMonster>(World, CharacterClass, SpawnLocation, SpawnRotation);
	if (!IsValid(Monster))
		return nullptr;

	Monster->Init(Preset);
	Monster->SpawnDefaultController();

	return Monster;
}

ARSMonster::ARSMonster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = ARSAIMonsterController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void ARSMonster::BeginPlay()
{
	Super::BeginPlay();
}

void ARSMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearHitStopTimer();
}

void ARSMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARSMonster::ApplyDamage(float Damage, AActor* Caster)
{
	Super::ApplyDamage(Damage, Caster);

	if (IsDead())
		return;

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetScalarParameterValueOnMaterials(FRSName::MatParam_HitSplash, 1.f);
	}
	CustomTimeDilation = 0.f;
	ClearHitStopTimer();
	GetWorldTimerManager().SetTimer(HitStopTimerHandle, this, &ARSMonster::OnHitStopTimeout, 5.f / 60.f);
}

void ARSMonster::ApplyDie(AActor* Caster)
{
	Super::ApplyDie(Caster);
	
	SetLifeSpan(3.f);

	const float DropRate = FMath::RandRange(0.f, 1.f);
	if (DropRate < 0.2f)
	{
		if (URSGlobalData* GlobalData = URSAssetManager::Get().GetOrLoadGlobalData())
		{
			const int32 FieldItemCount = GlobalData->FieldItems.Num();
			if (FieldItemCount > 0)
			{
				const int32 ItemIndex = FieldItemCount > 1 ? FMath::RandRange(0, GlobalData->FieldItems.Num() - 1) : 0;
				URSUtil::SpawnActor<ARSFieldItem>(GetWorld(), GlobalData->FieldItems[ItemIndex], GetActorLocation(), FRotator::ZeroRotator);
			}
		}
	}
}

void ARSMonster::OnHitStopTimeout()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetScalarParameterValueOnMaterials(FRSName::MatParam_HitSplash, 0);
	}
	CustomTimeDilation = 1.f;
	ClearHitStopTimer();
}

void ARSMonster::ClearHitStopTimer()
{
	if (HitStopTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(HitStopTimerHandle);
		HitStopTimerHandle.Invalidate();
	}
}
