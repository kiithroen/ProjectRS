// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Skill/RSSkillComponent.h"
#include "Component/RSStatComponent.h"
#include "RSType.h"
#include "Skill/RSSkill.h"
#include "Character/RSCharacterPreset.h"
#include "Component/RSMonsterMovementComponent.h"
#include "Component/RSHitBoxComponent.h"
#include "Data/RSSkillItem.h"
#include "GameFramework/RSAssetManager.h"

ARSCharacter::ARSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	}

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->RotationRate = FRotator(0.f, 720.f, 0.f);
		MovementComp->GravityScale = 3.f;
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		MeshComp->SetGenerateOverlapEvents(false);
		MeshComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		MeshComp->bUseAttachParentBound = true;
		MeshComp->bComponentUseFixedSkelBounds = true;
		MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
		MeshComp->bEnableUpdateRateOptimizations = true;
		MeshComp->KinematicBonesUpdateType = EKinematicBonesUpdateToPhysics::SkipAllBones;
		MeshComp->bConsiderAllBodiesForBounds = false;
	}

	HitBoxComp = CreateDefaultSubobject<URSHitBoxComponent>(TEXT("HitBox"));
	if (HitBoxComp)
	{
		HitBoxComp->SetupAttachment(GetCapsuleComponent());
		HitBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitBoxComp->SetCollisionProfileName(TEXT("HitBox"));
		HitBoxComp->SetGenerateOverlapEvents(false);
		HitBoxComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		HitBoxComp->bUseAttachParentBound = true;
	}

	SkillComp = CreateDefaultSubobject<URSSkillComponent>(TEXT("Skill"));
	StatComp = CreateDefaultSubobject<URSStatComponent>(TEXT("Stat"));
}

void ARSCharacter::Init(URSCharacterPreset* InPreset)
{
	if (!InPreset)
	{
		ensure(false);
		return;
	}

	Preset = InPreset;
	
	if (StatComp)
	{
		StatComp->Init(Preset->DefaultStatValues);
	}
	
	if (SkillComp)
	{
		SkillComp->Init();

		for (auto& SkillItemId : Preset->DefaultSkillItemIds)
		{
			const FRSSkillItem* SkillItem = URSAssetManager::Get().FindRowById<FRSSkillItem>(SkillItemId);
			if (!SkillItem)
			{
				ensure(false);
				continue;
			}
			
			URSSkill* Skill = NewObject<URSSkill>(this, SkillItem->SkillClass);
			if (!Skill)
				continue;
			
			Skill->SetLevel(SkillItem->InitialLevel);
			
			SkillComp->EquipSkill(Skill);
		}
	}
}

void ARSCharacter::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_CharacterTick);
	
	Super::Tick(DeltaTime);
	
	LastMovementDirection = GetActorForwardVector();
	
	UpdateMovementByCurve(DeltaTime);
}

void ARSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	LastMovementDirection = GetActorForwardVector();

	StatComp->OnStatValueChanged.AddUObject(this, &ARSCharacter::OnStatValueChanged);
}

void ARSCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	StatComp->OnStatValueChanged.RemoveAll(this);

	OnHeal.Clear();
	OnDamaged.Clear();
	OnDie.Clear();
}

bool ARSCharacter::IsDead() const
{
	if (!SkillComp)
		return false;
	
	return SkillComp->HasFlag(RSGT_Flag_Dead);
}

bool ARSCharacter::IsImmune() const
{
	if (!SkillComp)
		return false;
	
	return SkillComp->HasFlag(RSGT_Flag_Immune);
}

void ARSCharacter::ApplyHeal(float Heal, AActor* InCaster)
{
	if (!SkillComp)
		return;

	if (!StatComp)
		return;

	if (SkillComp->HasFlag(RSGT_Flag_Dead))
		return;

	StatComp->AddValue(RSGT_Stat_CurrentHP, Heal);

	OnHeal.Broadcast(this, Heal, InCaster);
}

void ARSCharacter::ApplyDamage(float Damage, AActor* InCaster)
{
	if (!SkillComp)
		return;

	if (!StatComp)
		return;

	if (SkillComp->HasFlag(RSGT_Flag_Dead))
		return;

	if (SkillComp->HasFlag(RSGT_Flag_Immune))
		return;
	
	StatComp->AddValue(RSGT_Stat_CurrentHP, -Damage);
	
	OnDamaged.Broadcast(this, Damage, InCaster);

	if (StatComp->GetValue(RSGT_Stat_CurrentHP) <= 0.f)
	{
		ApplyDie(InCaster);
	}
}

void ARSCharacter::Launch(const FVector& Velocity, AActor* Caster)
{
	if (!SkillComp)
		return;
	
	if (SkillComp->HasFlag(RSGT_Flag_Dead))
		return;

	if (SkillComp->HasFlag(RSGT_Flag_SuperArmor))
		return;

	LaunchCharacter(Velocity, true, false);
}

void ARSCharacter::ApplyDie(AActor* InCaster)
{
	if (!SkillComp)
		return;
	
	if (!StatComp)
		return;
	
	SkillComp->AddFlag(RSGT_Flag_Dead);
	SkillComp->ClearSkillEffectsByTypeTag(RSGT_SkillEffect_Type_Active, false);

	StatComp->SetValue(RSGT_Stat_CurrentHP, 0.f);
	
	StopMovementAll();
	EnableMovementInput(false);

	EnableGhost(true);

	OnDie.Broadcast(this, InCaster);
}

float ARSCharacter::GetStat(const FGameplayTag& Tag) const
{
	if (!StatComp)
		return 0.f;

	return StatComp->GetValue(Tag);
}

void ARSCharacter::SetStat(const FGameplayTag& Tag, float Value) const
{
	if (!StatComp)
		return;

	StatComp->SetValue(Tag, Value);
}

void ARSCharacter::AddStat(const FGameplayTag& Tag, float Value) const
{
	if (!StatComp)
		return;

	StatComp->AddValue(Tag, Value);
}

const FGameplayTag& ARSCharacter::GetId() const
{
	if (!Preset.IsValid())
	{
		ensure(false);
		return FGameplayTag::EmptyTag;
	}
	
	return Preset->Id;
}

void ARSCharacter::StartMovementByCurve(UCurveVector* Curve, const FVector& Direction, const FVector& Scale)
{
	if (!Curve)
	{
		ensure(false);
		return;
	}

	bMovementByCurve = true;
	MovementByCurveElapsedTime = 0.f;
	MovementCurve = Curve;
	MovementCurveDirection = Direction;
	MovementCurveScale = Scale;
}

void ARSCharacter::StopMovementByCurve()
{
	bMovementByCurve = false;
	MovementByCurveElapsedTime = 0.f;
	MovementCurve = nullptr;
	MovementCurveDirection = FVector::ForwardVector;
	MovementCurveScale = FVector::ZeroVector;
}

void ARSCharacter::StopMovementAll()
{
	StopMovementByCurve();
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
}

void ARSCharacter::EnableMovementInput(bool bEnable)
{
	AController* MyController = GetController();
	if (!MyController)
		return;

	MyController->SetIgnoreMoveInput(!bEnable);
}

void ARSCharacter::EnableGhost(bool bEnable)
{
	if (bEnable)
	{
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->SetCollisionProfileName(TEXT("Spectator"));
		}

		if (HitBoxComp)
		{
			HitBoxComp->SetCollisionProfileName(TEXT("NoCollision"));
		}
	}
	else
	{
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->SetCollisionProfileName(TEXT("Pawn"));
		}

		if (HitBoxComp)
		{
			HitBoxComp->SetCollisionProfileName(TEXT("HitBox"));
		}
	}
}

float ARSCharacter::PlayAnimMontageWithEnd(UAnimMontage* AnimMontage, TFunction<void(UAnimMontage*,bool)> EndCallback, float InPlayRate, FName StartSectionName)
{
	UAnimInstance * AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr; 
	if( AnimMontage && AnimInstance )
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			if( StartSectionName != NAME_None )
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			FOnMontageEnded EndDelegate;
			EndDelegate.BindLambda( EndCallback);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
			return Duration;
		}
	}	

	return 0.f;
}

FVector ARSCharacter::GetSocketLocation(FName SocketName) const
{
	if (UMeshComponent* MeshComp = FindComponentByClass<UMeshComponent>())
	{
		return MeshComp->GetSocketLocation(SocketName);
	}

	return GetActorLocation();
}

void ARSCharacter::OnStatValueChanged(URSStatComponent* StatComponent, const FGameplayTag& Tag, float OldValue, float NewValue)
{
	ensure(StatComp == StatComponent);
	if (!StatComp)
		return;

	if (Tag.MatchesTagExact(RSGT_Stat_CurrentHP))
	{
		if (StatComp->GetValue(RSGT_Stat_CurrentHP) > StatComp->GetValue(RSGT_Stat_MaxHP))
		{
			StatComp->SetValue(RSGT_Stat_CurrentHP, StatComp->GetValue(RSGT_Stat_MaxHP));
		}

		if (StatComp->GetValue(RSGT_Stat_CurrentHP) < 0)
		{
			StatComp->SetValue(RSGT_Stat_CurrentHP, 0);
		}
	}
	else if (Tag.MatchesTagExact(RSGT_Stat_CurrentMana))
	{
		if (StatComp->GetValue(RSGT_Stat_CurrentMana) > StatComp->GetValue(RSGT_Stat_MaxMana))
		{
			StatComp->SetValue(RSGT_Stat_CurrentMana, StatComp->GetValue(RSGT_Stat_MaxMana));
		}

		if (StatComp->GetValue(RSGT_Stat_CurrentMana) < 0)
		{
			StatComp->SetValue(RSGT_Stat_CurrentMana, 0);
		}
	}
	else if (Tag.MatchesTagExact(RSGT_Stat_MovementSpeed))
	{
		if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
		{
			MovementComp->MaxWalkSpeed = NewValue;
		}
	}
}

void ARSCharacter::UpdateMovementByCurve(float DeltaTime)
{
	if (!bMovementByCurve)
		return;

	if (!MovementCurve)
		return;

	MovementByCurveElapsedTime += DeltaTime;

	float MinTime = 0.f;
	float MaxTime = 0.f;
	MovementCurve->GetTimeRange(MinTime, MaxTime);

	if (MovementByCurveElapsedTime >= MaxTime)
	{
		StopMovementByCurve();
		return;
	}

	const FVector Velocity = MovementCurve->GetVectorValue(MovementByCurveElapsedTime) * MovementCurveScale;
	const FTransform ToDirection(MovementCurveDirection.Rotation(), GetActorLocation());
	const FVector DeltaLocation = ToDirection.TransformPosition(Velocity * DeltaTime);
	SetActorLocation(DeltaLocation, true);
}
