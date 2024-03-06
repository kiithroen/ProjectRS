// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RSGameMode.h"

#include "RSAssetManager.h"
#include "GameFramework/RSPlayerController.h"
#include "Character/RSHero.h"
#include "Character/RSMonster.h"
#include "Common/RSUtil.h"
#include "Data/RSGlobalData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Skill/RSSkillComponent.h"
#include "Kismet/GameplayStatics.h"

ARSGameMode::ARSGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ARSGameMode::StartPlay()
{
	Super::StartPlay();

	ARSPlayerController* PlayerController = Cast<ARSPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!IsValid(PlayerController))
		return;

	const AActor* PlayerStart = FindPlayerStart(PlayerController);
	if (!IsValid(PlayerStart))
	{
		ensure(false);
		return;
	}

	const URSGlobalData* GlobalData = URSAssetManager::Get().GetOrLoadGlobalData();
	if (!GlobalData)
		return;

	if (!GlobalData->DefaultHero)
	{
		ensure(false);
		return;
	}
	
	const FVector StartLocation = PlayerStart->GetActorLocation();
	ARSHero* Hero = ARSHero::Spawn(GetWorld(), GlobalData->DefaultHero, StartLocation, FRotator::ZeroRotator);
	if (!IsValid(Hero))
		return;

	PlayerController->Possess(Hero);

	for (int32 Index = 0; Index < GlobalData->InitialMonsterNum; ++Index)
	{
		SpawnMonster();
	}

	if (GlobalData->MonsterSpawnInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ARSGameMode::OnSpawnTimer, GlobalData->MonsterSpawnInterval, true);
	}
}

void ARSGameMode::Tick(float DeltaTime)
{
	if (CheckGameOver())
		return;
	
	UpdateMonster();
}

bool ARSGameMode::CheckGameOver()
{
	const ARSHero* Hero = Cast<ARSHero>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Hero))
		return true;
	
	bool bGameOver = false;
	if (const URSSkillComponent* SkillComp = Hero->FindComponentByClass<URSSkillComponent>())
	{
		if (SkillComp->HasFlag(RSGT_Flag_Dead))
		{
			bGameOver = true;
		}
	}

	if (bGameOver)
	{
		UGameplayStatics::SetGamePaused(this, true);
	}

	return bGameOver;
}

void ARSGameMode::OnSpawnTimer()
{
	const URSGlobalData* GlobalData = URSAssetManager::Get().GetOrLoadGlobalData();
	if (!GlobalData)
		return;
	
	if (SpawnedMonster.Num() >= GlobalData->MaxMonsterNum)
		return;

	SpawnMonster();

	UE_LOG(LogRS, Display, TEXT("Monster Num : %d"), SpawnedMonster.Num());
}

void ARSGameMode::UpdateMonster()
{
	SpawnedMonster.RemoveAllSwap([](const ARSMonster* Monster) {
		return !IsValid(Monster);
		});

	TeleportMonster();
}

void ARSGameMode::SpawnMonster()
{
	const URSGlobalData* GlobalData = URSAssetManager::Get().GetOrLoadGlobalData();
	if (!GlobalData)
		return;

	if (!GlobalData->DefaultMonster)
		return;

	
	FVector SpawnLocation;
	if (!FindRandomSpawnLocation(SpawnLocation))
		return;
	
	ARSMonster* Monster = ARSMonster::Spawn(GetWorld(), GlobalData->DefaultMonster, SpawnLocation, FRotator::ZeroRotator);
	if (!IsValid(Monster))
		return;

	SpawnedMonster.Emplace(Monster);
}

void ARSGameMode::TeleportMonster()
{
	ARSHero* Hero = Cast<ARSHero>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Hero))
		return;

	const URSGlobalData* GlobalData = URSAssetManager::Get().GetOrLoadGlobalData();
	if (!GlobalData)
		return;
	
	const FVector HeroLocation = Hero->GetActorLocation();
	for (const auto& Monster : SpawnedMonster)
	{
		const FVector CurLocation = Monster->GetActorLocation();

		const float DiffX = FMath::Abs(HeroLocation.X - CurLocation.X);
		const float DiffY = FMath::Abs(HeroLocation.Y - CurLocation.Y);

		if (DiffX > GlobalData->MonsterTeleportRange || DiffY > GlobalData->MonsterTeleportRange)
		{
			FVector TeleportLocation;
			if (FindRandomSpawnLocation(TeleportLocation))
			{
				Monster->TeleportTo(TeleportLocation, Monster->GetActorRotation(), false, true);
			}
		}
	}
}

bool ARSGameMode::FindRandomSpawnLocation(FVector& OutSpawnLocation) const
{
	const ARSHero* Hero = Cast<ARSHero>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Hero))
		return false;

	const URSGlobalData* GlobalData = URSAssetManager::Get().GetOrLoadGlobalData();
	if (!GlobalData)
		return false;
	
	const FVector HeroLocation = Hero->GetActorLocation();

	constexpr int32 MaxTry = 10;
	for (int32 Index = 0; Index < MaxTry; ++Index)
	{
		FVector RandomDirection = UKismetMathLibrary::RandomUnitVector();
		RandomDirection.Z = 0.f;
		RandomDirection.Normalize();

		const FVector SpawnDirection = FMath::Lerp(RandomDirection, Hero->GetLastMovementDirection(), 0.4f);
		const float RandomRange = UKismetMathLibrary::RandomFloatInRange(GlobalData->MonsterSpawnRangeMin, GlobalData->MonsterSpawnRangeMax);
		const FVector SpawnLocation = HeroLocation + SpawnDirection.GetSafeNormal() * RandomRange;
		if (IsSpawnableLocation(SpawnLocation))
		{
			OutSpawnLocation = SpawnLocation;
			return true;
		}
	}

	return false;
}


bool ARSGameMode::IsSpawnableLocation(const FVector& SpawnLocation) const
{
	UWorld* World = GetWorld();
	if (!World)
		return false;
	
	FHitResult HitResult;
	if (!World->LineTraceSingleByChannel(HitResult, SpawnLocation, SpawnLocation - 200.f*FVector::UpVector, ECC_WorldStatic))
		return false;
	
	return true;
}
