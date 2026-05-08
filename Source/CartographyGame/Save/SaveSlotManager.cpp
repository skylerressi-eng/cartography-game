// Copyright (c) 2026 The Last Cartographer.

#include "Save/SaveSlotManager.h"

#include "HAL/PlatformFileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Map/FieldMapComponent.h"
#include "Map/MapDrawingLibrary.h"
#include "Misc/Paths.h"
#include "Player/CartographerCharacter.h"
#include "Save/CartographySaveGame.h"
#include "Save/SaveLoadSubsystem.h"

namespace
{
	FString SlotsRoot() { return FPaths::ProjectSavedDir() / TEXT("SaveGames"); }
}

FString USaveSlotManager::GetThumbnailPath(const FString& SlotName) const
{
	return FPaths::ProjectSavedDir() / TEXT("MapImages") / FString::Printf(TEXT("%s_field.png"), *SlotName);
}

TArray<FSlotInfo> USaveSlotManager::ListSlots() const
{
	TArray<FSlotInfo> Out;
	IPlatformFile& PF = IPlatformFile::GetPlatformPhysical();
	TArray<FString> Files;
	PF.IterateDirectory(*SlotsRoot(), [&Files](const TCHAR* P, bool bIsDir)
	{
		if (!bIsDir) { Files.Add(P); }
		return true;
	});
	for (const FString& Path : Files)
	{
		if (!Path.EndsWith(TEXT(".sav"))) { continue; }
		FSlotInfo I;
		I.SlotName = FPaths::GetBaseFilename(Path);
		I.bIsAuto  = I.SlotName.Equals(TEXT("Auto"), ESearchCase::IgnoreCase);
		FFileStatData St = PF.GetStatData(*Path);
		I.SavedAt = St.ModificationTime;
		if (UCartographySaveGame* Save = Cast<UCartographySaveGame>(UGameplayStatics::LoadGameFromSlot(I.SlotName, 0)))
		{
			I.TimeOfDay = Save->TimeOfDay;
			I.DisplayName = Save->SlotName;
		}
		I.ThumbnailPath = GetThumbnailPath(I.SlotName);
		Out.Add(I);
	}
	Out.Sort([](const FSlotInfo& A, const FSlotInfo& B){ return A.SavedAt > B.SavedAt; });
	return Out;
}

bool USaveSlotManager::SaveNamed(const FString& SlotName, const FString& Display, APawn* Player, float TimeOfDay)
{
	if (USaveLoadSubsystem* SL = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>())
	{
		const bool bOk = SL->SaveToSlot(SlotName, Player, TimeOfDay);
		if (bOk && Display.Len() > 0)
		{
			if (UCartographySaveGame* S = Cast<UCartographySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
			{
				S->SlotName = Display;
				UGameplayStatics::SaveGameToSlot(S, SlotName, 0);
			}
		}
		return bOk;
	}
	return false;
}

bool USaveSlotManager::LoadNamed(const FString& SlotName, APawn* Player, float& OutTimeOfDay)
{
	if (USaveLoadSubsystem* SL = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>())
	{
		return SL->LoadFromSlot(SlotName, Player, OutTimeOfDay);
	}
	return false;
}

bool USaveSlotManager::DeleteSlot(const FString& SlotName)
{
	const bool bSav = UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	IPlatformFile& PF = IPlatformFile::GetPlatformPhysical();
	const FString F = FPaths::ProjectSavedDir() / TEXT("MapImages") / FString::Printf(TEXT("%s_field.png"), *SlotName);
	const FString M = FPaths::ProjectSavedDir() / TEXT("MapImages") / FString::Printf(TEXT("%s_master.png"), *SlotName);
	if (PF.FileExists(*F)) { PF.DeleteFile(*F); }
	if (PF.FileExists(*M)) { PF.DeleteFile(*M); }
	return bSav;
}

bool USaveSlotManager::QuickSave(APawn* Player, float TimeOfDay)
{
	return SaveNamed(TEXT("Quick"), TEXT("Quick Save"), Player, TimeOfDay);
}

bool USaveSlotManager::AutoSave(APawn* Player, float TimeOfDay)
{
	return SaveNamed(TEXT("Auto"), TEXT("Auto Save"), Player, TimeOfDay);
}
