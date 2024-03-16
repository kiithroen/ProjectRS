// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSMonster.h"

#include "RSCharacterPreset.h"
#include "GameFramework/RSAssetManager.h"
#include "Common/RsUtil.h"
#include "Actor/RSFieldItem.h"
#include "AI/RSAIMonsterController.h"
#include "Character/RSHero.h"
#include "Component/RSMonsterMovementComponent.h"
#include "Data/RSGlobalData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

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
	: Super(ObjectInitializer.SetDefaultSubobjectClass<URSMonsterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	AIControllerClass = ARSAIMonsterController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Disabled;

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetGenerateOverlapEvents(false);
	}
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bAlwaysCheckFloor = false;
		MovementComp->bEnablePhysicsInteraction = false;
		MovementComp->DefaultLandMovementMode = MOVE_NavWalking;
		MovementComp->bSweepWhileNavWalking = false;
		MovementComp->SetGroundMovementMode(MOVE_NavWalking);
		MovementComp->bUseRVOAvoidance = true;
		MovementComp->AvoidanceConsiderationRadius = 150.f;
	}
}

void ARSMonster::BeginPlay()
{
	Super::BeginPlay();
}

void ARSMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearHitStopTimer();
	
	Super::EndPlay(EndPlayReason);
}

void ARSMonster::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_MonsterTick);
	
	Super::Tick(DeltaTime);

	UpdateBuryCoprse(DeltaTime);
}

void ARSMonster::ApplyDamage(float Damage, AActor* Caster)
{
	Super::ApplyDamage(Damage, Caster);

	if (IsDead())
		return;
	
	StopMovementAll();
	
	if (Preset.IsValid())
	{
		if (UAnimMontage* Montage = Preset->HitMontage.LoadSynchronous())
		{
			EnableMovementInput(false);
			PlayMontageWithEnd(Montage, [this](UAnimMontage*,bool)
			{
				EnableMovementInput(true);
			});
		}
	}
	
	PlayHitStop();
}

void ARSMonster::ApplyDie(AActor* Caster)
{
	Super::ApplyDie(Caster);

	DeathLocation = GetActorLocation();
	FTimerHandle TimerHandle;;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		StartBuryCoprse();
	}), 3.f, false);
	
	PlayHitStop();
	
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

void ARSMonster::PlayHitStop()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetScalarParameterValueOnMaterials(FRSName::MatParam_HitSplash, 1.f);
		MeshComp->GlobalAnimRateScale = 0.1f;
	}
	
	ClearHitStopTimer();
	GetWorldTimerManager().SetTimer(HitStopTimerHandle, this, &ARSMonster::OnHitStopTimeout, 10.f / 60.f);
}

void ARSMonster::OnHitStopTimeout()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetScalarParameterValueOnMaterials(FRSName::MatParam_HitSplash, 0);
		MeshComp->GlobalAnimRateScale = 1.f;
	}
	
	ClearHitStopTimer();
}

void ARSMonster::StartBuryCoprse()
{
	bBuryCorpse = true;
	SetLifeSpan(2.f);
}

void ARSMonster::UpdateBuryCoprse(float DeltaTime)
{
	if (bBuryCorpse)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->AddWorldOffset(-2.f * GetSimpleCollisionRadius() * DeltaTime * FVector::UpVector);
		}
	}
}

void ARSMonster::ClearHitStopTimer()
{
	if (HitStopTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(HitStopTimerHandle);
		HitStopTimerHandle.Invalidate();
	}
}
