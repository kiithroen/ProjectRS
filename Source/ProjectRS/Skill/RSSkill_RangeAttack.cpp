// Fill out your copyright notice in the Description page of Project Settings.


#include "RSSkill_RangeAttack.h"
#include "Actor/RSProjectile.h"
#include "Skill/RSSkillEffect.h"
#include "RSType.h"
#include "Common/RSUtil.h"
#include "Character/RSMonster.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

URSSkill_RangeAttack::URSSkill_RangeAttack()
{
}

void URSSkill_RangeAttack::OnBegin()
{
	Super::OnBegin();

	RemainAmmoCount = AmmoCount;
	RemainShotTime = 0.f;
}

void URSSkill_RangeAttack::OnUpdate(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_SkillRangeAttackTick);
	
	Super::OnUpdate(DeltaTime);

	RemainShotTime = FMath::Max(RemainShotTime - DeltaTime, 0.f);
	if (RemainShotTime > 0)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	const URSSkillComponent* OwnerSkillComp = OwnerActor->FindComponentByClass<URSSkillComponent>();
	if (!OwnerSkillComp)
		return;

	if (OwnerSkillComp->HasFlag(RSGT_Flag_Dead))
		return;

	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	
	// 에이밍 방향은 모바일 대응을 위해 단순 이동방향으로
	FVector AimDirection;
	if (const ARSCharacter* OwnerCharacter = Cast<ARSCharacter>(OwnerActor))
	{
		AimDirection = OwnerCharacter->GetLastMovementDirection();
	}
	else
	{
		AimDirection = OwnerActor->GetVelocity().GetSafeNormal();
	}

	if (AimDirection == FVector::ZeroVector)
	{
		AimDirection = OwnerActor->GetActorForwardVector();
	}
	TArray<FHitResult> OutHitResults;
	FRSTargetInfo_SphereArea TargetInfo;
	TargetInfo.StartLocation = OwnerLocation;
	TargetInfo.EndLocation = OwnerLocation;
	TargetInfo.Radius = AttackRange;
	TargetInfo.TraceChannel = TraceChannel;
	
	TArray<AActor*> ActorsToIgnore;
	
	URSUtil::CollectTargets_SphereArea(OwnerActor, TargetInfo, ActorsToIgnore, OutHitResults);
	
	OutHitResults.RemoveAllSwap([&OwnerLocation, &AimDirection, this](const FHitResult& A) {
			AActor* Actor = A.GetActor();
			if (!IsValid(Actor))
				return true;

			const FVector ActorDirection = UKismetMathLibrary::GetDirectionUnitVector(OwnerLocation, Actor->GetActorLocation());
			if (FMath::Abs(URSUtil::FindAngle2D(AimDirection, ActorDirection)) > AimAngleRange * 0.5f)
				return true;
		
			return false;
		});

	if (OutHitResults.Num() == 0)
		return;
	
	OutHitResults.Sort([&OwnerLocation](const FHitResult& A, const FHitResult& B) {
		const AActor* ActorA = A.GetActor();
		const AActor* ActorB = B.GetActor();

		const float DistA = FVector::DistSquared(ActorA->GetActorLocation(), OwnerLocation);
		const float DistB = FVector::DistSquared(ActorB->GetActorLocation(), OwnerLocation);
		return DistA < DistB;
		});

	FVector SpawnLocation = OwnerLocation;
	if (const UMeshComponent* MeshComp = OwnerActor->FindComponentByClass<UMeshComponent>())
	{
		if (MeshComp->DoesSocketExist(ProjectileSocketName))
		{
			SpawnLocation = MeshComp->GetSocketLocation(ProjectileSocketName);
		}
	}

	const FRotator SpawnRotation = OwnerActor->GetActorRotation();

	const int32 ProjectileSpawnCount = ProjectileCount.AsInteger(GetLevel());
	for (int32 Index = 0; Index < ProjectileSpawnCount; ++Index)
	{
		SCOPE_CYCLE_COUNTER(STAT_ProjectileSpawn);
		
		ARSProjectile* Projectile = URSUtil::SpawnActor<ARSProjectile>(World, ProjectileClass, SpawnLocation, SpawnRotation);
		if (!IsValid(Projectile))
			continue;

		Projectile->SetCaster(OwnerActor);
		Projectile->AddIgnoreActor(OwnerActor);
		Projectile->SetTraceChannel(TraceChannel);
		Projectile->SetSpeed(ProjectileSpeed, ProjectileSpeed);

		if (bAutoAiming && Index < OutHitResults.Num())
		{
			const AActor* TargetActor = OutHitResults[Index].GetActor();
			ensure(IsValid(TargetActor));
			Projectile->SetHoming(TargetActor, 1.f);
		}
		else
		{
			if (Index == 0)
			{	
				Projectile->SetDirection(AimDirection);
			}
			else
			{
				// 첫발 외에는 랜덤하게 뿌린다
				// TODO: 일정간격으로 뿌리는거 추가? 
				const FVector RandomDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(AimDirection, AimAngleRange * 0.5f);
				Projectile->SetDirection(RandomDirection.GetSafeNormal2D());
			}
		}

		Projectile->SetExplosion(ExplosionLifeTime, ExplosionHitCount);
		Projectile->OnProjectileExplosion.AddUObject(this, &URSSkill_RangeAttack::OnProjectileExplosion);

		Projectile->OnProjectileHit.AddUObject(this, &URSSkill_RangeAttack::OnProjectileHit);
	}
	
	RemainShotTime = ShotInterval;
	RemainAmmoCount -= 1;
	if (RemainAmmoCount <= 0)
	{
		Deactivate();
	}
}

void URSSkill_RangeAttack::OnProjectileHit(ARSProjectile* Projectile, const TArray<FHitResult>& HitResults)
{
	const AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	for (auto& HitResult : HitResults)
	{
		const AActor* HitActor = HitResult.GetActor();
		if (!IsValid(HitActor))
			continue;

		URSSkillComponent* SkillComp = HitActor->FindComponentByClass<URSSkillComponent>();
		if (!SkillComp)
			continue;

		if (SkillComp->HasFlag(RSGT_Flag_Dead))
			continue;

		for (auto& SkillEffect : SkillEffectOnHit)
		{
			SkillComp->AddSkillEffect(SkillEffect, GetLevel(), GetOwnerActor(), Projectile, &HitResult);
		}
	}
}

void URSSkill_RangeAttack::OnProjectileExplosion(ARSProjectile* Projectile, int32 HitCount)
{
	// TODO: 폭발시 스플래시 처리
}