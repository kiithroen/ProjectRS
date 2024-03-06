// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/RSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/RSPlayerController.h"
#include "GameFramework/RSAssetManager.h"
#include "GameFramework/RSGameMode.h"

void URSGameInstance::Init()
{
	URSAssetManager::Get().OnInitGameInstance();
}
