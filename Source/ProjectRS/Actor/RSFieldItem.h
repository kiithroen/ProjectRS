// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSFieldItem.generated.h"

class UBoxComponent;
class URSSkillEffect;
class UPaperFlipbookComponent;

UCLASS()
class PROJECTRS_API ARSFieldItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ARSFieldItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void OnHit(AActor* Actor);

protected:
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "FieldItem")
	TArray<TObjectPtr<URSSkillEffect>> SkillEffectOnHit;

	UPROPERTY(EditDefaultsOnly, Category = "FieldItem")
	int32 SkillEffectLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "FieldItem")
	float MagneticDistance = 200.f;

private:
	UPROPERTY(VisibleAnywhere, Category = "RS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};