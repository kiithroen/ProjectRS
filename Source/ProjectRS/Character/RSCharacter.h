// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Interface/RSCombatInterface.h"
#include "RSCharacter.generated.h"

class URSStatComponent;
class URSSkillComponent;
class UBoxComponent;
class URSCharacterPreset;
class ARSCharacter;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FRSOnHeal, ARSCharacter*, float, AActor*);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FRSOnDamaged, ARSCharacter*, float, AActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FRSOnDie, ARSCharacter*, AActor*);

/**
 * 
 */
UCLASS(Abstract)
class PROJECTRS_API ARSCharacter : public ACharacter, public IRSCombatInterface
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

	virtual float GetStat(const FGameplayTag& Tag) const override;
	virtual void SetStat(const FGameplayTag& Tag, float Value) const override;
	virtual void AddStat(const FGameplayTag& Tag, float Value) const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "IsDead"))
	bool K2_IsDead() const { return IsDead(); }
	
	const FGameplayTag& GetId() const;
	FVector GetLastMovementDirection() const { return LastMovementDirection; }
	
	void Init(URSCharacterPreset* InPreset);
	
	void StopMovementAll();
	void StartMovementByCurve(UCurveVector* Curve, const FVector& Direction, const FVector& Scale);
	void StopMovementByCurve();
	void EnableMovementInput(bool bEnable);

	void EnableGhost(bool bEnable);

	float PlayAnimMontageWithEnd(UAnimMontage* AnimMontage, TFunction<void(UAnimMontage*,bool)> EndCallback, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	
	FVector GetSocketLocation(FName SocketName) const;

	FORCEINLINE UBoxComponent* GetHitBox() const { return HitBoxComp; }
	FORCEINLINE URSSkillComponent* GetSkill() const { return SkillComp; }
	FORCEINLINE URSStatComponent* GetStat() const { return StatComp; }

protected:
	UFUNCTION()
	void OnStatValueChanged(URSStatComponent* StatComponent, const FGameplayTag& Tag, float OldValue, float NewValue);

	void UpdateMovementByCurve(float DeltaTime);

public:
	FRSOnHeal OnHeal;
	FRSOnDamaged OnDamaged;
	FRSOnDie OnDie;
	
	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> HitBoxComp;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSSkillComponent> SkillComp;

	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSStatComponent> StatComp;
	
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
