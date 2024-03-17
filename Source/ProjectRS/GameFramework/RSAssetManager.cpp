// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RSAssetManager.h"

#include "Data/RSGlobalData.h"
#include "Setting/RSGameSetting.h"

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

	check(false);

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

void URSAssetManager::OnInitGameInstance()
{
	LoadGlobalData();
}
