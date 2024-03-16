// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "RSType.h"
#include "RSAssetManager.generated.h"

class URSObjectPool;
class URSGlobalData;
class URSDataTableManager;

/**
 * 
 */
UCLASS(Config = Game)
class PROJECTRS_API URSAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static const FPrimaryAssetType	FieldItemType;
	static const FPrimaryAssetType	UsableItemType;
	static const FPrimaryAssetType	EquipItemType;
	static const FPrimaryAssetType	TokenItemType;
	static const FPrimaryAssetType	SkillItemType;
	
	static const FPrimaryAssetType	CharacterPreset;
	
	static URSAssetManager& Get();

public:
	template <typename T>
	T* ForceLoadPrimaryData(const FPrimaryAssetId& AssetId) {
		if (!AssetId.IsValid())
		{
			ensure(false);
			return nullptr;
		}
		
		const FSoftObjectPath ItemPath = GetPrimaryAssetPath(AssetId);

		return Cast<T>(ItemPath.TryLoad());
	}

	template <typename T>
	T* FindRow(const FName& RowName)
	{
		if (const UScriptStruct* RowStruct = T::StaticStruct())
		{
			const FName RowStructName = RowStruct->GetFName();
			if (GlobalDataTables.Contains(RowStructName))
			{
				if (T* Data = GlobalDataTables[RowStructName]->FindRow<T>(RowName, *RowStructName.ToString(), false))
				{
					return Data;
				}

				ensure(false);
				UE_LOG(LogRS, Warning, TEXT("Cant Find RowName(%s) in %s"), *RowName.ToString(), *RowStructName.ToString());
			}

			ensure(false);
			UE_LOG(LogRS, Warning, TEXT("Cant Find Table(%s)"), *RowStructName.ToString());
		}

		ensure(false);
		return nullptr;
	}

	template <typename T>
	T* FindRowById(const FGameplayTag& Id)
	{
		const FName RowName = *Id.ToString();
		if (T* Data = FindRow<T>(RowName))
		{
			if (Data->Id != Id)
			{
				UE_LOG(LogRS, Warning, TEXT("Cant Match RowName(%s)=Id(%s)"), *RowName.ToString(), *Data->Id.ToString());
				return nullptr;
			}

			return Data;
		}
		
		return nullptr;
	}
	
	URSGlobalData* GetOrLoadGlobalData();
	URSObjectPool& GetObjectPool();
	void InitObjectPool();

	void OnInitGameInstance();

private:
	URSGlobalData* LoadGlobalData();

	UPROPERTY(Transient)
	TObjectPtr<URSGlobalData> GlobalData;
	
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UDataTable>> GlobalDataTables;

	UPROPERTY(Transient, VisibleAnywhere, Category = "RS")
	TObjectPtr<URSObjectPool> ObjectPool;
};
