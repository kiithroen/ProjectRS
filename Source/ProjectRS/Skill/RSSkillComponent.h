// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RSSkillComponent.generated.h"

class URSSkillComponent;
class URSSkill;
class URSSkillEffect;

DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnFlagAdded, URSSkillComponent*, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnFlagRemoved, URSSkillComponent*, FGameplayTag);

UCLASS()
class PROJECTRS_API URSSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URSSkillComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init();
	
	void EquipSkill(URSSkill* Skill);
	void UnEquipSkill(const FGameplayTag& Id);
	void ClearSkills();
	
	URSSkill* FindSkill(const FGameplayTag& Id) const;
	TArray<URSSkill*> FindSkillsByTypeTag(const FGameplayTag& Tag, bool bExact = true) const;
	TArray<URSSkill*> FindSkillsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact = true) const;
	TArray<URSSkill*> FindSkillsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact = true) const;
	
	bool TryCastSkill(const FGameplayTag& Id);
	void StopSkill(const FGameplayTag& Id);
	bool CanActivateSkill(const FGameplayTag& Id) const;
	
	void SendEvent(const FGameplayTag& EventTag);
	
	void AddFlag(const FGameplayTag& Tag);
	void RemoveFlag(const FGameplayTag& Tag);
	bool HasFlag(const FGameplayTag& Tag, bool bExact = true) const;
	void AddFlags(const FGameplayTagContainer& Tags);
	void RemoveFlags(const FGameplayTagContainer& Tags);
	bool HasFlagAny(const FGameplayTagContainer& Tags, bool bExact = true) const;
	bool HasFlagAll(const FGameplayTagContainer& Tags, bool bExact = true) const;

	URSSkillEffect* AddSkillEffect(const URSSkillEffect* SkillEffect, int32 Level, AActor* Caster, AActor* Causer = nullptr, const FHitResult* HitResult = nullptr);
	void RemoveSkillEffect(URSSkillEffect* SkillEffect);
	void ClearSkillEffect(URSSkillEffect* SkillEffect);
	URSSkillEffect* FindSkillEffectByStackGroupTag(const FGameplayTag& StackGroupTag) const;
	void RemoveSkillEffectsByTypeTag(const FGameplayTag& Tag, bool bExact = true);
	void RemoveSkillEffectsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact = true);
	void RemoveSkillEffectsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact = true);
	void ClearSkillEffectsByTypeTag(const FGameplayTag& Tag, bool bExact = true);
	void ClearSkillEffectsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact = true);
	void ClearSkillEffectsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact = true);
	TArray<URSSkillEffect*> FindSkillEffectsByTypeTag(const FGameplayTag& Tag, bool bExact = true) const;
	TArray<URSSkillEffect*> FindSkillEffectsByTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact = true) const;
	TArray<URSSkillEffect*> FindSkillEffectsByTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact = true) const;
	
private:
	void UpdateSkills(float DeltaTime);
	void UpdateSkillEffects(float DeltaTime);

public:
	FRSOnFlagAdded OnFlagAdded;
	FRSOnFlagRemoved OnFlagRemoved;

private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<URSSkill>> Skills;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<URSSkillEffect>> ActiveSkillEffects;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<URSSkillEffect>> PendingSkillEffects;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TArray<TObjectPtr<URSSkillEffect>> ExpiredSkillEffects;
	
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FGameplayTagContainer Flags;
};
