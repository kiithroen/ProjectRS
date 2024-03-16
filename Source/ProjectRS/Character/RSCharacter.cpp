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

	HitBoxComponent = CreateDefaultSubobject<URSHitBoxComponent>(TEXT("HitBox"));
	if (HitBoxComponent)
	{
		HitBoxComponent->SetupAttachment(GetCapsuleComponent());
		HitBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitBoxComponent->SetCollisionProfileName(TEXT("HitBox"));
		HitBoxComponent->SetGenerateOverlapEvents(false);
		HitBoxComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		HitBoxComponent->bUseAttachParentBound = true;
	}

	SkillComponent = CreateDefaultSubobject<URSSkillComponent>(TEXT("Skill"));
	StatComponent = CreateDefaultSubobject<URSStatComponent>(TEXT("Stat"));
}

void ARSCharacter::Init(URSCharacterPreset* InPreset)
{
	if (!InPreset)
	{
		ensure(false);
		return;
	}

	Preset = InPreset;
	
	if (StatComponent)
	{
		StatComponent->Init(Preset->DefaultStatValues);
	}
	
	if (SkillComponent)
	{
		SkillComponent->Init();

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
			
			SkillComponent->EquipSkill(Skill);
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

	StatComponent->OnStatValueChanged.AddUObject(this, &ARSCharacter::OnStatValueChanged);
}

void ARSCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StatComponent->OnStatValueChanged.RemoveAll(this);

	OnHeal.Clear();
	OnDamaged.Clear();
	OnDie.Clear();
	
	Super::EndPlay(EndPlayReason);
}

bool ARSCharacter::IsDead() const
{
	if (!SkillComponent)
		return false;
	
	return SkillComponent->HasFlag(RSGT_Flag_Dead);
}

bool ARSCharacter::IsImmune() const
{
	if (!SkillComponent)
		return false;
	
	return SkillComponent->HasFlag(RSGT_Flag_Immune);
}

void ARSCharacter::ApplyHeal(float Heal, AActor* InCaster)
{
	if (!SkillComponent)
		return;

	if (!StatComponent)
		return;

	if (SkillComponent->HasFlag(RSGT_Flag_Dead))
		return;

	StatComponent->AddValue(RSGT_Stat_CurrentHP, Heal);

	OnHeal.Broadcast(this, Heal, InCaster);
}

void ARSCharacter::ApplyDamage(float Damage, AActor* InCaster)
{
	if (!SkillComponent)
		return;

	if (!StatComponent)
		return;

	if (SkillComponent->HasFlag(RSGT_Flag_Dead))
		return;

	if (SkillComponent->HasFlag(RSGT_Flag_Immune))
		return;
	
	StatComponent->AddValue(RSGT_Stat_CurrentHP, -Damage);
	
	OnDamaged.Broadcast(this, Damage, InCaster);

	if (StatComponent->GetValue(RSGT_Stat_CurrentHP) <= 0.f)
	{
		ApplyDie(InCaster);
	}
}

void ARSCharacter::ApplyDie(AActor* InCaster)
{
	if (!SkillComponent)
		return;
	
	if (!StatComponent)
		return;
	
	SkillComponent->AddFlag(RSGT_Flag_Dead);
	SkillComponent->ClearSkillEffectsByTypeTag(RSGT_SkillEffect_Type_Active, false);

	StatComponent->SetValue(RSGT_Stat_CurrentHP, 0.f);
	
	StopMovementAll();
	EnableMovementInput(false);

	EnableGhost(true);

	OnDie.Broadcast(this, InCaster);
}

void ARSCharacter::Launch(const FVector& Velocity, AActor* Caster)
{
	if (!SkillComponent)
		return;
	
	if (SkillComponent->HasFlag(RSGT_Flag_Dead))
		return;

	if (SkillComponent->HasFlag(RSGT_Flag_SuperArmor))
		return;

	LaunchCharacter(Velocity, true, false);
}

void ARSCharacter::StopMovementAll()
{
	StopMovementByCurve();
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
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

void ARSCharacter::EnableMovementInput(bool bEnable)
{
	AController* MyController = GetController();
	if (!MyController)
		return;

	MyController->SetIgnoreMoveInput(!bEnable);
}

FVector ARSCharacter::GetLastMovementDirection() const
{
	return LastMovementDirection;
}

void ARSCharacter::EnableGhost(bool bEnable)
{
	if (bEnable)
	{
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->SetCollisionProfileName(TEXT("Spectator"));
		}

		if (HitBoxComponent)
		{
			HitBoxComponent->SetCollisionProfileName(TEXT("NoCollision"));
		}
	}
	else
	{
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->SetCollisionProfileName(TEXT("Pawn"));
		}

		if (HitBoxComponent)
		{
			HitBoxComponent->SetCollisionProfileName(TEXT("HitBox"));
		}
	}
}

float ARSCharacter::GetStat(const FGameplayTag& Tag) const
{
	if (!StatComponent)
		return 0.f;

	return StatComponent->GetValue(Tag);
}

void ARSCharacter::SetStat(const FGameplayTag& Tag, float Value) const
{
	if (!StatComponent)
		return;

	StatComponent->SetValue(Tag, Value);
}

void ARSCharacter::AddStat(const FGameplayTag& Tag, float Value) const
{
	if (!StatComponent)
		return;

	StatComponent->AddValue(Tag, Value);
}

bool ARSCharacter::UseSkillSlot(const FGameplayTag& Slot)
{
	if (!SkillComponent)
		return false;

	if (!Preset.IsValid())
		return false;
	
	if (!Preset->SlotToSkillId.Contains(Slot))
		return false;

	return SkillComponent->TryCastSkill(Preset->SlotToSkillId[Slot]);
}

float ARSCharacter::PlayMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
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

			return Duration;
		}
	}	

	return 0.f;
}

float ARSCharacter::PlayMontageWithEnd(UAnimMontage* AnimMontage, TFunction<void(UAnimMontage*,bool)> EndCallback, float InPlayRate, FName StartSectionName)
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

const FGameplayTag& ARSCharacter::GetId() const
{
	if (!Preset.IsValid())
	{
		ensure(false);
		return FGameplayTag::EmptyTag;
	}
	
	return Preset->Id;
}

void ARSCharacter::OnStatValueChanged(URSStatComponent* StatComp, const FGameplayTag& Tag, float OldValue, float NewValue)
{
	ensure(StatComp == StatComponent);
	if (!StatComponent)
		return;

	if (Tag.MatchesTagExact(RSGT_Stat_CurrentHP))
	{
		if (StatComponent->GetValue(RSGT_Stat_CurrentHP) > StatComponent->GetValue(RSGT_Stat_MaxHP))
		{
			StatComponent->SetValue(RSGT_Stat_CurrentHP, StatComponent->GetValue(RSGT_Stat_MaxHP));
		}

		if (StatComponent->GetValue(RSGT_Stat_CurrentHP) < 0)
		{
			StatComponent->SetValue(RSGT_Stat_CurrentHP, 0);
		}
	}
	else if (Tag.MatchesTagExact(RSGT_Stat_CurrentMana))
	{
		if (StatComponent->GetValue(RSGT_Stat_CurrentMana) > StatComponent->GetValue(RSGT_Stat_MaxMana))
		{
			StatComponent->SetValue(RSGT_Stat_CurrentMana, StatComponent->GetValue(RSGT_Stat_MaxMana));
		}

		if (StatComponent->GetValue(RSGT_Stat_CurrentMana) < 0)
		{
			StatComponent->SetValue(RSGT_Stat_CurrentMana, 0);
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
