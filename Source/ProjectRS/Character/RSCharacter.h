// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interface/RSAnimInterface.h"
#include "Interface/RSCombatInterface.h"
#include "RSCharacter.generated.h"

class URSHitBoxComponent;
class URSStatComponent;
class URSSkillComponent;
class URSCharacterPreset;
class ARSCharacter;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FRSOnHeal, ARSCharacter*, float, AActor*);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FRSOnDamaged, ARSCharacter*, float, AActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnDie, ARSCharacter*, AActor*);

/**
 * 
 */
UCLASS(Abstract)
class PROJECTRS_API ARSCharacter : public ACharacter, public IRSCombatInterface, public IRSAnimInterface
{
	GENERATED_BODY()
	
public:
	ARSCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual bool IsDead() const override;
	virtual bool IsImmune() const override;
	virtual void ApplyHeal(float Heal, AActor* Caster) override;
	virtual void ApplyDamage(float Damage, AActor* Caster) override;
	virtual void ApplyDie(AActor* Caster) override;
	
	virtual void Launch(const FVector& Velocity, AActor* Caster) override;
	
	virtual void StopMovementAll() override;
	virtual void StartMovementByCurve(UCurveVector* Curve, const FVector& Direction, const FVector& Scale) override;
	virtual void StopMovementByCurve() override;
	virtual void EnableMovementInput(bool bEnable) override;
	virtual FVector GetLastMovementDirection() const override;

	virtual void EnableGhost(bool bEnable) override;

	virtual float GetStat(const FGameplayTag& Tag) const override;
	virtual void SetStat(const FGameplayTag& Tag, float Value) const override;
	virtual void AddStat(const FGameplayTag& Tag, float Value) const override;

	virtual bool UseSkillSlot(const FGameplayTag& Slot) override;
	
	virtual float PlayMontage(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
	virtual float PlayMontageWithEnd(UAnimMontage* AnimMontage, TFunction<void(UAnimMontage*,bool)> EndCallback, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "IsDead"))
	bool K2_IsDead() const { return IsDead(); }
	
	const FGameplayTag& GetId() const;
	
	void Init(URSCharacterPreset* InPreset);

	FORCEINLINE URSHitBoxComponent* GetHitBox() const { return HitBoxComponent; }
	FORCEINLINE URSSkillComponent* GetSkill() const { return SkillComponent; }
	FORCEINLINE URSStatComponent* GetStat() const { return StatComponent; }

protected:
	UFUNCTION()
	void OnStatValueChanged(URSStatComponent* StatComp, const FGameplayTag& Tag, float OldValue, float NewValue);

	void UpdateMovementByCurve(float DeltaTime);

public:
	FRSOnHeal OnHeal;
	FRSOnDamaged OnDamaged;
	FRSOnDie OnDie;
	
	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSHitBoxComponent> HitBoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSSkillComponent> SkillComponent;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSStatComponent> StatComponent;
	
	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	bool bMovementByCurve = false;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	float MovementByCurveElapsedTime = 0.f;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FVector MovementCurveScale = FVector::OneVector;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FVector MovementCurveDirection = FVector::ForwardVector;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TObjectPtr<UCurveVector> MovementCurve;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FVector LastMovementDirection;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	FTimerHandle DamagedTimerHandle;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TWeakObjectPtr<URSCharacterPreset> Preset;
};
