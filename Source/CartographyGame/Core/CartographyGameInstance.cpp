// Copyright (c) 2026 The Last Cartographer.

#include "Core/CartographyGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Save/SaveSlotManager.h"
#include "Settings/CartoUserSettings.h"

void UCartographyGameInstance::Init()
{
	Super::Init();
	if (UCartoUserSettings* S = UCartoUserSettings::Get())
	{
		S->LoadSettings();
		S->ApplySettings(false);
	}
}

void UCartographyGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UCartographyGameInstance::RequestNewGame()
{
	UGameplayStatics::OpenLevel(this, TEXT("L_Island"));
}

void UCartographyGameInstance::RequestContinue()
{
	UGameplayStatics::OpenLevel(this, TEXT("L_Island"), true, TEXT("?Slot=Auto"));
}
