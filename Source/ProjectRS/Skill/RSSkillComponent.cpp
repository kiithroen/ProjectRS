// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/RSSkillComponent.h"
#include "Skill/RSSkill.h"
#include "Skill/RSSkillEffect.h"
#include "Common/RSUtil.h"
#include "System/RSAggregatingTickSubsystem.h"

URSSkillComponent::URSSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
	{
		PrimaryComponentTick.UnRegisterTickFunction();
		AggregatingTickSubsystem->RegisterComponent(this, TG_DuringPhysics);
	}
	
}

void URSSkillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URSAggregatingTickSubsystem* AggregatingTickSubsystem = URSAggregatingTickSubsystem::Get(GetWorld()))
	{
		AggregatingTickSubsystem->UnRegisterComponent(this);
	}

	OnFlagAdded.Clear();
	OnFlagRemoved.Clear();
	
	Super::EndPlay(EndPlayReason);
}

void URSSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_SkillTick);
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateSkills(DeltaTime);
	UpdateSkillEffects(DeltaTime);
}

void URSSkillComponent::Init()
{
	ClearSkills();
}

void URSSkillComponent::EquipSkill(URSSkill* Skill)
{
	if (!Skill)
		return;

	if (!Skill->GetId().IsValid())
	{
		ensure(false);
		return;
	}

	if (FindSkill(Skill->GetId()))
	{
		ensure(false);
		return;
	}


	Skill->Init(this);
	Skill->OnEquip();
	
	Skills.Emplace(Skill);
}

void URSSkillComponent::UnEquipSkill(const FGameplayTag& Id)
{
	URSSkill* Skill = FindSkill(Id);
	if (!Skill)
		return;

	Skill->OnEquip();
	Skills.RemoveSingleSwap(Skill);
}

void URSSkillComponent::ClearSkills()
{
	for (auto& Skill : Skills)
	{
		if (Skill)
		{
			Skill->OnEquip();
		}
	}

	Skills.Reset();
}

URSSkill* URSSkillComponent::FindSkill(const FGameplayTag& Id) const
{
	if (!Id.IsValid())
		return nullptr;
	
	for (auto& Skill : Skills)
	{
		if (Skill && Skill->GetId().MatchesTagExact(Id))
			return Skill;
	}

	return nullptr;
}


TArray<URSSkill*> URSSkillComponent::FindSkillsByTypeTag(const FGameplayTag& Tag, bool bExact) const
{
	TArray<URSSkill*> Results;
	if (!Tag.IsValid())
		return MoveTemp(Results);
	
	for (auto& Skill : Skills)
	{
		if (Skill && Skill->HasTypeTag(Tag, bExact))
		{
			Results.Emplace(Skill);
		}
	}

	return MoveTemp(Results);
}

TArray<URSSkill*> URSSkillComponent::FindSkillsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact) const
{
	TArray<URSSkill*> Results;
	if (!Tags.IsValid())
		return MoveTemp(Results);
	
	for (auto& Skill : Skills)
	{
		if (Skill && Skill->HasTypeTagsAny(Tags, bExact))
		{
			Results.Emplace(Skill);
		}
	}

	return MoveTemp(Results);
}

TArray<URSSkill*> URSSkillComponent::FindSkillsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact) const
{
	TArray<URSSkill*> Results;
	if (!Tags.IsValid())
		return MoveTemp(Results);
	
	for (auto& Skill : Skills)
	{
		if (Skill && Skill->HasTypeTagsAll(Tags, bExact))
		{
			Results.Emplace(Skill);
		}
	}

	return MoveTemp(Results);
}

bool URSSkillComponent::CanActivateSkill(const FGameplayTag& Id) const
{
	const URSSkill* Skill = FindSkill(Id);
	if (!Skill)
		return false;

	return Skill->CanActivate();
}

bool URSSkillComponent::TryCastSkill(const FGameplayTag& Id)
{
	URSSkill* Skill = FindSkill(Id);
	if (!Skill)
		return false;

	if (!Skill->CanActivate())
		return false;

	Skill->OnBegin();

	return true;
}

void URSSkillComponent::StopSkill(const FGameplayTag& Id)
{
	URSSkill* Skill = FindSkill(Id);
	if (!Skill)
		return;

	Skill->EndSkill();
}

void URSSkillComponent::SendEvent(const FGameplayTag& EventTag)
{
	for (auto& Skill : Skills)
	{
		if (Skill && Skill->IsActive())
		{
			Skill->OnEvent(EventTag);
		}
	}
}

void URSSkillComponent::AddFlag(const FGameplayTag& Tag)
{
	if (!Flags.HasTagExact(Tag))
	{
		Flags.AddTag(Tag);
		OnFlagAdded.Broadcast(this, Tag);
	}
}


void URSSkillComponent::RemoveFlag(const FGameplayTag& Tag)
{
	if (Flags.HasTagExact(Tag))
	{
		Flags.RemoveTag(Tag);
		OnFlagRemoved.Broadcast(this, Tag);
	}
}


bool URSSkillComponent::HasFlag(const FGameplayTag& Tag, bool bExact) const
{
	if (bExact)
	{
		return Flags.HasTagExact(Tag);
	}

	return Flags.HasTag(Tag);
}


void URSSkillComponent::AddFlags(const FGameplayTagContainer& Tags)
{
	for (auto& Tag : Tags)
	{
		AddFlag(Tag);
	}
}

void URSSkillComponent::RemoveFlags(const FGameplayTagContainer& Tags)
{
	for (auto& Tag : Tags)
	{
		RemoveFlag(Tag);
	}
}

bool URSSkillComponent::HasFlagAny(const FGameplayTagContainer& Tags, bool bExact) const
{
	if (bExact)
	{
		return Flags.HasAnyExact(Tags);
	}

	return Flags.HasAny(Tags);
}

bool URSSkillComponent::HasFlagAll(const FGameplayTagContainer& Tags, bool bExact) const
{
	if (bExact)
	{
		return Flags.HasAllExact(Tags);
	}

	return Flags.HasAll(Tags);
}

URSSkillEffect* URSSkillComponent::AddSkillEffect(const URSSkillEffect* SkillEffect, int32 Level, AActor* Caster, AActor* Causer, const FHitResult* HitResult)
{
	if (Level < SkillEffect->GetRequiredLevel())
		return nullptr;

	const int32 Target = URSUtil::MTRandRange(1, 10000);
	if (Target > SkillEffect->GetChanceToAdd())
		return nullptr;

	const FGameplayTag& StackGroupTag = SkillEffect->GetStackGroupTag();
	if (StackGroupTag.IsValid())
	{
		if (URSSkillEffect* OldSkillEffectInSameStackGroup = FindSkillEffectByStackGroupTag(SkillEffect->GetStackGroupTag()))
		{
			if (OldSkillEffectInSameStackGroup->IsStackable())
			{
				// stack
				OldSkillEffectInSameStackGroup->AddStackCount(1);
				OldSkillEffectInSameStackGroup->ResetRemainTime();
				return nullptr;
			}
			else
			{
				// Repeat, Infinite
				if (OldSkillEffectInSameStackGroup->GetExecutionType() != ERSSkillEffectExecutionType::Once)
				{
					if (OldSkillEffectInSameStackGroup->GetGrade() > SkillEffect->GetGrade())
					{
						// ignore
						return nullptr;
					}
				}
			}
		}
	}
	
	URSSkillEffect* NewSkillEffect = SkillEffect->Clone();
	if (!NewSkillEffect)
		return nullptr;

	NewSkillEffect->Init(this);
	NewSkillEffect->SetLevel(Level);
	NewSkillEffect->AddCaster(Caster);
	NewSkillEffect->AddCauser(Causer);
	if (HitResult)
	{
		NewSkillEffect->AddHitResult(*HitResult);
	}
	
	PendingSkillEffects.Emplace(NewSkillEffect);
	
	return nullptr;
}

void URSSkillComponent::RemoveSkillEffect(URSSkillEffect* SkillEffect)
{
	if (!SkillEffect)
		return;
	
	SkillEffect->AddStackCount(-1);

	if (SkillEffect->GetStackCount() == 0)
	{
		ExpiredSkillEffects.Emplace(SkillEffect);
	}
}

void URSSkillComponent::ClearSkillEffect(URSSkillEffect* SkillEffect)
{
	if (!SkillEffect)
		return;

	ExpiredSkillEffects.Emplace(SkillEffect);
}

void URSSkillComponent::RemoveSkillEffectsByTypeTag(const FGameplayTag& Tag, bool bExact)
{
	const TArray<URSSkillEffect*> Results = FindSkillEffectsByTypeTag(Tag, bExact);
	for (auto& SkillEffect : Results)
	{
		RemoveSkillEffect(SkillEffect);
	}
}

void URSSkillComponent::RemoveSkillEffectsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact)
{
	const TArray<URSSkillEffect*> Results = FindSkillEffectsByTypeTagsAny(Tags, bExact);
	for (auto& SkillEffect : Results)
	{
		RemoveSkillEffect(SkillEffect);
	}
}

void URSSkillComponent::RemoveSkillEffectsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact)
{
	const TArray<URSSkillEffect*> Results = FindSkillEffectsByTypeTagsAll(Tags, bExact);
	for (auto& SkillEffect : Results)
	{
		RemoveSkillEffect(SkillEffect);
	}
}

void URSSkillComponent::ClearSkillEffectsByTypeTag(const FGameplayTag& Tag, bool bExact)
{
	const TArray<URSSkillEffect*> Results = FindSkillEffectsByTypeTag(Tag, bExact);
	for (auto& SkillEffect : Results)
	{
		ClearSkillEffect(SkillEffect);
	}
}

void URSSkillComponent::ClearSkillEffectsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact)
{
	const TArray<URSSkillEffect*> Results = FindSkillEffectsByTypeTagsAny(Tags, bExact);
	for (auto& SkillEffect : Results)
	{
		ClearSkillEffect(SkillEffect);
	}
}

void URSSkillComponent::ClearSkillEffectsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact)
{
	const TArray<URSSkillEffect*> Results = FindSkillEffectsByTypeTagsAll(Tags, bExact);
	for (auto& SkillEffect : Results)
	{
		ClearSkillEffect(SkillEffect);
	}
}

TArray<URSSkillEffect*> URSSkillComponent::FindSkillEffectsByTypeTag(const FGameplayTag& Tag, bool bExact) const
{
	TArray<URSSkillEffect*> Results;
	if (!Tag.IsValid())
		return MoveTemp(Results);
	
	for (auto& SkillEffect : ActiveSkillEffects)
	{
		if (SkillEffect && SkillEffect->HasTypeTag(Tag, bExact))
		{
			Results.Emplace(SkillEffect);
		}
	}

	return MoveTemp(Results);
}

TArray<URSSkillEffect*> URSSkillComponent::FindSkillEffectsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact) const
{
	TArray<URSSkillEffect*> Results;
	if (!Tags.IsValid())
		return MoveTemp(Results);
	
	for (auto& SkillEffect : ActiveSkillEffects)
	{
		if (SkillEffect && SkillEffect->HasTypeTagsAny(Tags, bExact))
		{
			Results.Emplace(SkillEffect);
		}
	}

	return MoveTemp(Results);
}

TArray<URSSkillEffect*> URSSkillComponent::FindSkillEffectsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact) const
{
	TArray<URSSkillEffect*> Results;
	for (auto& SkillEffect : ActiveSkillEffects)
	{
		if (SkillEffect && SkillEffect->HasTypeTagsAll(Tags, bExact))
		{
			Results.Emplace(SkillEffect);
		}
	}

	return MoveTemp(Results);
}

URSSkillEffect* URSSkillComponent::FindSkillEffectByStackGroupTag(const FGameplayTag& StackGroupTag) const
{
	if (!StackGroupTag.IsValid())
		return nullptr;
	
	for (auto& SkillEffect : ActiveSkillEffects)
	{
		if (SkillEffect && SkillEffect->GetStackGroupTag().MatchesTagExact(StackGroupTag))
		{
			return SkillEffect;
		}
	}
	return nullptr;
}

void URSSkillComponent::UpdateSkills(float DeltaTime)
{
	for (auto& Skill : Skills)
	{
		Skill->Tick(DeltaTime);
		
		if (!Skill->IsActive())
		{
			if (Skill->IsAutoCast())
			{
				TryCastSkill(Skill->GetId());
			}
		}
	}
}

void URSSkillComponent::UpdateSkillEffects(float DeltaTime)
{
	for (auto& SkillEffect : PendingSkillEffects)
	{
		if (SkillEffect)
		{
			SkillEffect->OnAdd();
			SkillEffect->AddStackCount(1);
		}
	}
	ActiveSkillEffects.Append(PendingSkillEffects);
	PendingSkillEffects.Reset();
	
	for (auto& SkillEffect : ActiveSkillEffects)
	{
		if (SkillEffect)
		{
			SkillEffect->Tick(DeltaTime);

			if (SkillEffect->IsExpired())
			{
				ExpiredSkillEffects.Emplace(SkillEffect);
			}
		}
	}
	
	for (auto& SkillEffect : ExpiredSkillEffects)
	{
		SkillEffect->OnRemove();
		SkillEffect->Release();
		ActiveSkillEffects.RemoveSingleSwap(SkillEffect);
	}
	ExpiredSkillEffects.Reset();
}