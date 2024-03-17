// Fill out your copyright notice in the Description page of Project Settings.


#include "RSObjectPoolSubsystem.h"

#include "Skill/RSSkillEffect_Damage.h"

URSObjectPoolSubsystem& URSObjectPoolSubsystem::Get()
{
	check(GEngine);

	if (URSObjectPoolSubsystem* Singleton = GEngine->GetEngineSubsystem<URSObjectPoolSubsystem>())
	{
		return *Singleton;
	}

	check(false);

	return *NewObject<URSObjectPoolSubsystem>();
}

void URSObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;

	InitPool<URSSkillEffect_Damage>(8, 64, 4);
}

void URSObjectPoolSubsystem::Deinitialize()
{
	ResetPool<URSSkillEffect_Damage>();
	
	check(bInitialized);
	bInitialized = false;
}

void URSObjectPoolSubsystem::SetDefaultSize(int32 InitSize, int32 MaxSize, int32 GrowSize)
{
	DefaultInitSize = InitSize;
	DefaultMaxSize = MaxSize;
	DefaultGrowSize = GrowSize;
}
