// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RSSkillComponent.h"
#include "RSSkill.generated.h"

class UWorld;
class URSSkillComponent;
class UCurveVector;

UCLASS(Abstract, Blueprintable)
class PROJECTRS_API URSSkill : public UObject
{
	GENERATED_BODY()
public:
	URSSkill();
	
	virtual UWorld* GetWorld() const override;
	
	virtual bool CanCast() const;
	virtual void Do(float DeltaTime);
	virtual void OnBegin();
	virtual void OnEnd();
	virtual void OnEquip();
	virtual void OnUnEquip();
	virtual void OnEvent(const FGameplayTag& EventTag);
	
	void Init(URSSkillComponent* InComponent);
	void Tick(float DeltaTime);

	URSSkillComponent* GetOwnerComponent() const;
	AActor* GetOwnerActor() const;
	AController* GetOwnerController() const;
	
	const FGameplayTag& GetId() const;
	int32 GetRequiredLevel() const;
	
	bool HasTypeTag(const FGameplayTag& Tag, bool bExact) const;
	bool HasTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact) const;
	bool HasTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact) const;

	float GetElapsedTime() const;
	
	bool IsCoolTimeExpired() const;
	float GetRemainCoolTime() const;
	void StartCoolTime();
	void SetRemainCoolTime(float InRemainCoolTime);
	void ResetCoolTime();
	float GetCoolTimeProgress() const;
	
	int32 GetLevel() const;
	void SetLevel(int32 InLevel);
	void ChangeLevel(int32 InLevel);
	
	bool IsEnoughCost() const;
	void ConsumeCost();

    bool IsAutoCast() const;
	bool IsActive() const;
	void EndSkill();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skill", meta = (Categories = "Skill.Id"))
	FGameplayTag Id;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill", meta = (Categories = "Skill.Type"))
	FGameplayTagContainer TypeTags;

	UPROPERTY(EditDefaultsOnly, Category = "Skill", meta = (Categories = "Flag"))
	FGameplayTagContainer Flags;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	int32 RequiredLevel = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CoolTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill", meta = (Categories = "Stat", ForceInlineRow))
	TMap<FGameplayTag, float> Costs;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	bool bAutoCast = false;
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Skill")
	TArray<TObjectPtr<URSSkillEffect>> SkillEffectsOnEquip;
	
private:
	TWeakObjectPtr<URSSkillComponent> OwnerComponent;
	TArray<TWeakObjectPtr<URSSkillEffect>> EquippedSkillEffects;

	int32 Level = 0;
	double ElapsedTime = 0;
	float RemainCoolTime = 0;
	bool bActive = false;
};