// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/RSSkill.h"

#include "RSSkillEffect.h"
#include "Skill/RSSkillComponent.h"
#include "GameFramework/PlayerController.h"
#include "Component/RSStatComponent.h"

URSSkill::URSSkill()
{
	
}

UWorld* URSSkill::GetWorld() const
{
	AActor* OwnerActor = GetOwnerActor();
	if (IsValid(OwnerActor))
		return OwnerActor->GetWorld();

	return nullptr;
}

bool URSSkill::CanActivate() const
{
	if (Level < RequiredLevel)
		return false;

	if (IsActive())
		return false;
	
	if (!IsEnoughCost())
		return false;
	
	if (!IsCoolTimeExpired())
		return false;

	return true;
}

void URSSkill::Do(float DeltaTime)
{
}

void URSSkill::OnBegin()
{
	const UWorld* World = GetWorld();
	if (!World)
		return;

	if (!OwnerComponent.IsValid())
		return;

	ElapsedTime = 0.f;

	if (Flags.IsValid())
	{
		OwnerComponent->AddFlags(Flags);
	}

	ConsumeCost();

	bActive = true;
}

void URSSkill::OnEnd()
{
	if (!OwnerComponent.IsValid())
		return;

	if (Flags.IsValid())
	{
		OwnerComponent->RemoveFlags(Flags);
	}

	StartCoolTime();
	
	bActive = false;
}

void URSSkill::OnEquip()
{
	for (const auto& SkillEffect : SkillEffectsOnEquip)
	{
		if (URSSkillEffect* NewSkillEffect = OwnerComponent->AddSkillEffect(SkillEffect, Level, OwnerComponent->GetOwner()))
		{
			EquippedSkillEffects.Add(NewSkillEffect);
		}
	}
}

void URSSkill::OnUnEquip()
{
	for (const auto& SkillEffect : EquippedSkillEffects)
	{
		OwnerComponent->RemoveSkillEffect(SkillEffect);
	}

	EquippedSkillEffects.Reset();
}

void URSSkill::OnEvent(const FGameplayTag& EventTag)
{
}

void URSSkill::Init(URSSkillComponent* InComponent)
{
	OwnerComponent = InComponent;
	RemainCoolTime = 0.f;
}

void URSSkill::Tick(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	if (!IsCoolTimeExpired())
	{
		RemainCoolTime = FMath::Max(RemainCoolTime - DeltaTime, 0.f);
	}
	
	if (IsActive())
	{
		Do(DeltaTime);
	}
}

URSSkillComponent* URSSkill::GetOwnerComponent() const
{
	return OwnerComponent.Get();
}

AActor* URSSkill::GetOwnerActor() const
{
	if (!OwnerComponent.IsValid())
		return nullptr;

	return OwnerComponent->GetOwner();
}

AController* URSSkill::GetOwnerController() const
{
	const APawn* MyPawn = Cast<APawn>(GetOwnerActor());
	if (!IsValid(MyPawn))
		return nullptr;

	return MyPawn->GetController();
}

const FGameplayTag& URSSkill::GetId() const
{
	return Id;
}

int32 URSSkill::GetRequiredLevel() const
{
	return RequiredLevel;
}

bool URSSkill::HasTypeTag(const FGameplayTag& Tag, bool bExact) const
{
	if (bExact)
	{
		return TypeTags.HasTagExact(Tag);
	}

	return TypeTags.HasTag(Tag);
}

bool URSSkill::HasTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact) const
{
	if (bExact)
	{
		return TypeTags.HasAnyExact(Tags);
	}

	return TypeTags.HasAny(Tags);
}

bool URSSkill::HasTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact) const
{
	if (bExact)
	{
		return TypeTags.HasAllExact(Tags);
	}

	return TypeTags.HasAll(Tags);
}

float URSSkill::GetElapsedTime() const
{
	return ElapsedTime;
}

bool URSSkill::IsCoolTimeExpired() const
{
	if (CoolTime == 0.f)
		return true;

	if (RemainCoolTime <= 0.f)
		return true;

	return false;
}

float URSSkill::GetRemainCoolTime() const
{
	return RemainCoolTime;
}

void URSSkill::StartCoolTime()
{
	if (CoolTime > 0.f)
	{
		SetRemainCoolTime(CoolTime);
	}
}

void URSSkill::SetRemainCoolTime(float InRemainCoolTime)
{
	RemainCoolTime = FMath::Clamp(InRemainCoolTime, 0.f, CoolTime);
}

void URSSkill::ResetCoolTime()
{
	SetRemainCoolTime(0.f);
}

float URSSkill::GetCoolTimeProgress() const
{
	if (CoolTime == 0.f)
		return 1.f;

	if (RemainCoolTime <= 0.f)
		return 1.f;

	return 1.f - RemainCoolTime / CoolTime;
}

int32 URSSkill::GetLevel() const
{
	return Level;
}

void URSSkill::SetLevel(int32 InLevel)
{
	Level = InLevel;
}

void URSSkill::ChangeLevel(int32 InLevel)
{
	SetLevel(InLevel);
	OnUnEquip();
	OnEquip();
}

bool URSSkill::IsEnoughCost() const
{
	const AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return false;

	const URSStatComponent* OwnerStat = OwnerActor->FindComponentByClass<URSStatComponent>();
	if (!OwnerStat)
		return false;

	for (auto& Cost : Costs)
	{
		if (OwnerStat->GetValue(Cost.Key) < Cost.Value)
			return false;
	}

	return true;
}

void URSSkill::ConsumeCost()
{
	if (!IsEnoughCost())
		return;

	const AActor* OwnerActor = GetOwnerActor();
	if (!IsValid(OwnerActor))
		return;

	URSStatComponent* OwnerStat = OwnerActor->FindComponentByClass<URSStatComponent>();
	if (!OwnerStat)
		return;

	for (auto& Item : Costs)
	{
		OwnerStat->AddValue(Item.Key, -Item.Value);
	}
}

bool URSSkill::IsAutoCast() const
{
	return bAutoCast;
}

void URSSkill::EndSkill()
{
	bActive = false;

	OnEnd();
}

bool URSSkill::IsActive() const
{
	return bActive;
}

