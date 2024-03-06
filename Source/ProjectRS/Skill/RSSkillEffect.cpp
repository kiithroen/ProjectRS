// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill/RSSkillEffect.h"

#include "Skill/RSSkillComponent.h"

URSSkillEffect::URSSkillEffect()
{
	
}

UWorld* URSSkillEffect::GetWorld() const
{
	const AActor* OwnerActor = GetOwnerActor();
	if (IsValid(OwnerActor))
		return OwnerActor->GetWorld();

	return nullptr;
}

URSSkillEffect* URSSkillEffect::Clone() const
{
	// TODO : Object Pool
	return DuplicateObject<URSSkillEffect>(this, GetOwnerComponent());
}

void URSSkillEffect::Do(float DeltaTime)
{
}

void URSSkillEffect::AddStackCount(int32 Count)
{
	if (!IsStackable())
		return;

	const int32 OldStackCount = StackCount;

	StackCount = StackCount + Count;
	if (StackCount <= 0)
	{
		StackCount = 0;
	}

	if (StackCount >= MaxStackCount)
	{
		StackCount = MaxStackCount;
	}

	OnChangeStack(OldStackCount, StackCount);
}

void URSSkillEffect::RemoveStackCount(int32 Count)
{
	AddStackCount(-Count);
}


void URSSkillEffect::OnAdd()
{
	RemainTime = DurationTime;
}

void URSSkillEffect::OnRemove()
{
}

void URSSkillEffect::OnRepeat()
{
}

void URSSkillEffect::OnChangeStack(int32 OldCount, int32 NewCount)
{
}

void URSSkillEffect::OnEvent(const FGameplayTag& EventTag)
{
}

void URSSkillEffect::Init(URSSkillComponent* InComponent)
{
	OwnerComponent = InComponent;
	RepeatCount = 0;
	StackCount = 0;
}

void URSSkillEffect::Tick(float DeltaTime)
{
	if (ExecutionType == ERSSkillEffectExecutionType::Infinite)
	{
		// nothing to do
	}
	else if (ExecutionType == ERSSkillEffectExecutionType::Repeat)
	{
		RemainTime -= DeltaTime;
		if (RemainTime <= 0.f)
		{
			OnRepeat();

			++RepeatCount;
			RemainTime += DurationTime;
		}
	}
	else
	{
		RemainTime -= DeltaTime;
		if (RemainTime <= 0.f)
		{
			RemainTime = 0.f;
		}
	}

	Do(DeltaTime);
}

void URSSkillEffect::SetCaster(AActor* InCaster)
{
	Caster = InCaster;
}

AActor* URSSkillEffect::GetCaster() const
{
	return Caster.Get();
}

void URSSkillEffect::AddHitResult(const FHitResult& InHitResult)
{
	check(!HitResult.IsValid());
	HitResult = MakeShared<FHitResult>(InHitResult);
}

FHitResult* URSSkillEffect::GetHitResult() const
{
	return HitResult.Get();
}

URSSkillComponent* URSSkillEffect::GetOwnerComponent() const
{ 
	return OwnerComponent.Get();
}

AActor* URSSkillEffect::GetOwnerActor() const
{
	if (!OwnerComponent.IsValid())
		return nullptr;

	return OwnerComponent->GetOwner();
}

AController* URSSkillEffect::GetOwnerController() const
{
	const APawn* MyPawn = Cast<APawn>(GetOwnerActor());
	if (!IsValid(MyPawn))
		return nullptr;

	return MyPawn->GetController();
}

int32 URSSkillEffect::GetRequiredLevel() const
{
	return RequiredLevel;
}

bool URSSkillEffect::HasTypeTag(const FGameplayTag& Tag, bool bExact) const
{
	if (bExact)
	{
		return TypeTags.HasTagExact(Tag);
	}

	return TypeTags.HasTag(Tag);
}

bool URSSkillEffect::HasTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact) const
{
	if (bExact)
	{
		return TypeTags.HasAnyExact(Tags);
	}

	return TypeTags.HasAny(Tags);
}

bool URSSkillEffect::HasTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact) const
{
	if (bExact)
	{
		return TypeTags.HasAllExact(Tags);
	}

	return TypeTags.HasAll(Tags);
}

const FGameplayTag& URSSkillEffect::GetStackGroupTag() const
{
	return StackGroupTag;
}

ERSSkillEffectExecutionType URSSkillEffect::GetExecutionType() const
{
	return ExecutionType;
}

int32 URSSkillEffect::GetGrade() const
{
	return Grade;
}

int32 URSSkillEffect::GetRepeatCount() const
{
	return RepeatCount;
}

int32 URSSkillEffect::GetLevel() const
{
	return Level;
}

void URSSkillEffect::SetLevel(int32 InLevel)
{
	Level = InLevel;
}

void URSSkillEffect::ChangeLevel(int32 InLevel)
{
	SetLevel(InLevel);
}

bool URSSkillEffect::IsStackable() const
{
	return MaxStackCount > 1;
}

int32 URSSkillEffect::GetStackCount() const
{
	return StackCount;
}

int32 URSSkillEffect::GetMaxStackCount() const
{
	return MaxStackCount;
}

float URSSkillEffect::GetDurationTime() const
{
	return DurationTime;
}

int32 URSSkillEffect::GetChanceToAdd() const
{
	return ChanceToAdd;
}

void URSSkillEffect::ResetRemainTime()
{
	if (ExecutionType == ERSSkillEffectExecutionType::Once)
	{
		RemainTime = DurationTime;
	}
}

bool URSSkillEffect::IsExpired() const
{
	if (ExecutionType == ERSSkillEffectExecutionType::Infinite)
	{
		return false;
	}
	else if (ExecutionType == ERSSkillEffectExecutionType::Repeat)
	{
		if (MaxRepeatCount > 0)
		{
			if (RepeatCount < MaxRepeatCount)
				return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (RemainTime > 0)
			return false;
	}

	return true;
}
