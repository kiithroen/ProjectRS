// Fill out your copyright notice in the Description page of Project Settings.


#include "RSGameSetting.h"

const URSGameSetting* URSGameSetting::Get()
{
	return GetDefault<URSGameSetting>();
}
