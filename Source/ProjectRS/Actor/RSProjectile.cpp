// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RSProjectile.h"

#include "Common/RSUtil.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ARSProjectile::ARSProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	if (SphereComp)
	{
		SphereComp->InitSphereRadius(15.0f);
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComp->SetCollisionProfileName(TEXT("NoCollision"));
		SphereComp->SetGenerateOverlapEvents(false);
		SphereComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		RootComponent = SphereComp;
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (MeshComp)
	{
		MeshComp->SetupAttachment(RootComponent);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		MeshComp->SetGenerateOverlapEvents(false);
		MeshComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	}

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->SetUpdatedComponent(SphereComp);
		ProjectileMovementComp->InitialSpeed = 3000.0f;
		ProjectileMovementComp->MaxSpeed = 3000.0f;
		ProjectileMovementComp->bRotationFollowsVelocity = true;
		ProjectileMovementComp->bShouldBounce = false;
		ProjectileMovementComp->Bounciness = 0.0f;
		ProjectileMovementComp->ProjectileGravityScale = 0.0f;
	}
}

void ARSProjectile::BeginPlay()
{
	Super::BeginPlay();

	PrevLocation = GetActorLocation();
}

void ARSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SphereComp)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;
	
	const float Radius = SphereComp->GetScaledSphereRadius();
	const FVector StartLocation = PrevLocation;
	const FVector EndLocation = GetActorLocation();

	const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActors(ActorsToIgnore);

	TArray<FHitResult> OutHitResults;
	World->SweepMultiByChannel(OutHitResults, StartLocation, EndLocation, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(Radius), Params);
	
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
