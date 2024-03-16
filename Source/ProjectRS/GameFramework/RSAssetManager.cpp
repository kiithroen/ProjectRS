// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RSAssetManager.h"

#include "Common/RSObjectPool.h"
#include "Data/RSGlobalData.h"
#include "Setting/RSGameSetting.h"
#include "Skill/RSSkillEffect_Damage.h"

const FPrimaryAssetType	URSAssetManager::FieldItemType = TEXT("RSFieldItemData");
const FPrimaryAssetType	URSAssetManager::UsableItemType = TEXT("RSUsableItemData");
const FPrimaryAssetType	URSAssetManager::EquipItemType = TEXT("RSEquipItemData");
const FPrimaryAssetType	URSAssetManager::TokenItemType = TEXT("RSTokenItemData");
const FPrimaryAssetType	URSAssetManager::SkillItemType = TEXT("RSSkillItemData");

const FPrimaryAssetType	URSAssetManager::CharacterPreset = TEXT("CharacterPreset");

URSAssetManager& URSAssetManager::Get()
{
	check(GEngine);

	if (URSAssetManager* Singleton = Cast<URSAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	ensure(false);

	return *NewObject<URSAssetManager>();
}

URSGlobalData* URSAssetManager::LoadGlobalData()
{
	if (!GlobalData)
	{
		if (const URSGameSetting* Setting = URSGameSetting::Get())
		{
			GlobalData = Setting->GlobalDataPath.LoadSynchronous();
			if (GlobalData)
			{
				for (auto& DataTable : GlobalData->DataTables)
				{
					if (const UScriptStruct* RowStruct = DataTable->GetRowStruct())
					{
						GlobalDataTables.Emplace(RowStruct->GetFName(), DataTable);
					}
				}
			}
		}
	}

	ensure(GlobalData);
	return GlobalData;
}

URSGlobalData* URSAssetManager::GetOrLoadGlobalData() 
{ 
	LoadGlobalData();

	return GlobalData;
}

URSObjectPool& URSAssetManager::GetObjectPool()
{
	ensure(ObjectPool);
	return *ObjectPool;
}

void URSAssetManager::InitObjectPool()
{
	ObjectPool = NewObject<URSObjectPool>();
	ObjectPool->SetDefaultSize(4, 16, 2);
	ObjectPool->Initialize<URSSkillEffect_Damage>(8, 64, 4);
}

void URSAssetManager::OnInitGameInstance()
{
	LoadGlobalData();

	InitObjectPool();
}
