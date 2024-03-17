// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RSGameInstance.h"
#include "GameFramework/RSAssetManager.h"

void URSGameInstance::Init()
{
	URSAssetManager::Get().OnInitGameInstance();
}
