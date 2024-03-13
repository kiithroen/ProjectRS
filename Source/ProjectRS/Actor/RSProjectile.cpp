// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RSProjectile.h"

#include "RSType.h"
#include "Common/RSUtil.h"
#include "Component/RSProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ARSProjectile::ARSProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	if (SphereComponent)
	{
		SphereComponent->InitSphereRadius(15.0f);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionProfileName(TEXT("NoCollision"));
		SphereComponent->SetGenerateOverlapEvents(false);
		SphereComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		RootComponent = SphereComponent;
	}

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (MeshComponent)
	{
		MeshComponent->SetupAttachment(RootComponent);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
		MeshComponent->SetGenerateOverlapEvents(false);
		MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	}

	ProjectileMovementComp = CreateDefaultSubobject<URSProjectileMovementComponent>(TEXT("ProjectileMovement"));
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->SetUpdatedComponent(SphereComponent);
		ProjectileMovementComp->InitialSpeed = 3000.0f;
		ProjectileMovementComp->MaxSpeed = 3000.0f;
		ProjectileMovementComp->bRotationFollowsVelocity = true;
		ProjectileMovementComp->bShouldBounce = false;
		ProjectileMovementComp->Bounciness = 0.0f;
		ProjectileMovementComp->ProjectileGravityScale = 0.0f;
		ProjectileMovementComp->bSweepCollision = false;
	}
}

void ARSProjectile::BeginPlay()
{
	Super::BeginPlay();

	PrevLocation = GetActorLocation();
}

void ARSProjectile::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_ProjectileTick);
	
	Super::Tick(DeltaTime);

	if (!SphereComponent)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;
	
	TArray<FHitResult> OutHitResults;
	FRSTargetInfo_SphereArea TargetInfo;
	TargetInfo.StartLocation = GetActorLocation();
	TargetInfo.EndLocation = GetActorLocation();
	TargetInfo.Radius = SphereComponent->GetScaledSphereRadius();
	TargetInfo.TraceChannel = TraceChannel;
	URSUtil::CollectTargets_SphereArea(this, TargetInfo, ActorsToIgnore, OutHitResults);
	
	if (OutHitResults.Num() > 0)
	{
		OnProjectileHit.Broadcast(this, OutHitResults);

		++HitCount; // 동시에 맞는거도 그냥 한번 맞는거로 처리

		if (ExplosionHitCount > 0)
		{
			if (HitCount == ExplosionHitCount)
			{
				OnExplosion();
			}
		}

		// 두번 맞지 않게
		for (const auto& HitResult : OutHitResults)
		{
			ActorsToIgnore.AddUnique(HitResult.GetActor()); 
		}
	}

	PrevLocation = GetActorLocation();
}

void ARSProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearExplosionTimer();

	OnProjectileHit.Clear();
	OnProjectileExplosion.Clear();
}

void ARSProjectile::SetCaster(AActor* InCaster)
{
	Caster = InCaster;
}

AActor* ARSProjectile::GetCaster() const
{
	return Caster.Get();
}

void ARSProjectile::AddIgnoreActor(AActor* Actor)
{
	ActorsToIgnore.AddUnique(Actor);
}

void ARSProjectile::SetTraceChannel(TEnumAsByte<ETraceTypeQuery> InTraceChannel)
{
	TraceChannel = InTraceChannel;
}

void ARSProjectile::SetSpeed(float InitialSpeed, float MaxSpeed)
{
	ProjectileMovementComp->InitialSpeed = InitialSpeed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
}

void ARSProjectile::SetHoming(const AActor* Target, float AccelMagnitude)
{
	if (!IsValid(Target))
	{
		ensure(false);
		return;
	}

	if (!Target->GetRootComponent())
	{
		ensure(false);
		return;
	}

	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->bIsHomingProjectile = true;
		ProjectileMovementComp->HomingAccelerationMagnitude = AccelMagnitude;
		ProjectileMovementComp->HomingTargetComponent = Target->GetRootComponent();

		const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Target->GetRootComponent()->GetComponentLocation());
		ProjectileMovementComp->Velocity = ProjectileMovementComp->InitialSpeed * Direction;
	}
}

void ARSProjectile::SetDirection(const FVector& Direction)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->bIsHomingProjectile = false;
		ProjectileMovementComp->HomingAccelerationMagnitude = 0.f;
		ProjectileMovementComp->HomingTargetComponent = nullptr;

		ProjectileMovementComp->Velocity = ProjectileMovementComp->InitialSpeed * Direction;
	}
}

void ARSProjectile::SetExplosion(float LifeTime, int32 InExplosionHitCount)
{
	if (LifeTime > 0.0f)
	{
		GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ARSProjectile::OnExplosion, LifeTime);
	}

	ExplosionHitCount = InExplosionHitCount;
}

void ARSProjectile::OnExplosion()
{
	ClearExplosionTimer();

	OnProjectileExplosion.Broadcast(this, HitCount);

	Destroy();
}

void ARSProjectile::ClearExplosionTimer()
{
	if (ExplosionTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);
		ExplosionTimerHandle.Invalidate();
	}
}
