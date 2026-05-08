// Copyright (c) 2026 The Last Cartographer.

#include "Settings/CartoUserSettings.h"

#include "Engine/Engine.h"

UCartoUserSettings::UCartoUserSettings() {}

UCartoUserSettings* UCartoUserSettings::Get()
{
	return Cast<UCartoUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UCartoUserSettings::SaveAndApply()
{
	ApplySettings(false);
	SaveSettings();
}
