// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RSType.h"
#include "RSSkillComponent.h"
#include "RSSkillEffect.generated.h"

class URSSkillEffectData;
class UWorld;
class URSSkillComponent;
class ARSPlayerController;

UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class PROJECTRS_API URSSkillEffect : public UObject
{
	GENERATED_BODY()
public:
	URSSkillEffect();
	
	virtual UWorld* GetWorld() const override;
	virtual URSSkillEffect* Clone() const;
	
	virtual void Do(float DeltaTime);
	virtual void OnAdd();
	virtual void OnRemove();
	virtual void OnRepeat();
	virtual void OnChangeStack(int32 OldCount, int32 NewCount);
	virtual void OnEvent(const FGameplayTag& EventTag);
	
	void Init(URSSkillComponent* InComponent);
	void Tick(float DeltaTime);

	void AddCaster(AActor* Caster);
	AActor* GetCaster() const;

	void AddCauser(AActor* Causer);
	AActor* GetCauser() const;
	
	void AddHitResult(const FHitResult& HitResult);
	FHitResult* GetHitResult() const;

	URSSkillComponent* GetOwnerComponent() const;
	AActor* GetOwnerActor() const;
	AController* GetOwnerController() const;
	int32 GetRequiredLevel() const;
	
	bool HasTypeTag(const FGameplayTag& Tag, bool bExact) const;
	bool HasTypeTagsAny(const FGameplayTagContainer& Tags, bool bExact) const;
	bool HasTypeTagsAll(const FGameplayTagContainer& Tags, bool bExact) const;

	const FGameplayTag& GetStackGroupTag() const;
	ERSSkillEffectExecutionType GetExecutionType() const;
	int32 GetGrade() const;
	int32 GetRepeatCount() const;
	
	int32 GetLevel() const;
	void SetLevel(int32 InLevel);
	void ChangeLevel(int32 InLevel);

	bool IsStackable() const;
	int32 GetStackCount() const;
	int32 GetMaxStackCount() const;
	void AddStackCount(int32 Count);
	void RemoveStackCount(int32 Count);
	
	float GetDurationTime() const;
	int32 GetChanceToAdd() const;
	void ResetRemainTime();
	
	bool IsExpired() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect")
	int32 RequiredLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect", meta = (Categories = "SkillEffect.Type"))
	FGameplayTagContainer TypeTags;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect", meta = (Categories = "SkillEffect.StackGroup"))
	FGameplayTag StackGroupTag;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect")
	ERSSkillEffectExecutionType ExecutionType = ERSSkillEffectExecutionType::Once;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect")
	int32 Grade = 1;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect", meta = (EditCondition = "ExecutionType == ERSSkillEffectExecutionType::Repeat", EditConditionHides))
	int32 MaxRepeatCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect")
	int32 MaxStackCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect", meta = (EditCondition = "ExecutionType != ERSSkillEffectExecutionType::Infinite", EditConditionHides))
	float DurationTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEffect")
	int32 ChanceToAdd = 10000;
	
private:
	TWeakObjectPtr<URSSkillComponent> OwnerComponent;
	TWeakObjectPtr<AActor> Caster;
	TWeakObjectPtr<AActor> Causer;
	TSharedPtr<FHitResult>	HitResult;
	
	int32 Level = 0;
	int32 RepeatCount = 0;
	int32 StackCount = 0;
	float RemainTime = 0.f;
};