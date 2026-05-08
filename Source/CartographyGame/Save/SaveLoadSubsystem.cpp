// Copyright (c) 2026 The Last Cartographer.

#include "Save/SaveLoadSubsystem.h"

#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Map/FieldMapComponent.h"
#include "Map/MapDrawingLibrary.h"
#include "Misc/Paths.h"
#include "Notebook/FieldNotebookComponent.h"
#include "Player/CartographerCharacter.h"
#include "Progression/ProgressionSubsystem.h"
#include "Save/CartographySaveGame.h"
#include "World/WorldResponseSubsystem.h"

FString USaveLoadSubsystem::GetSlotImagePath(const FString& SlotName, bool bMaster) const
{
	const FString File = FString::Printf(TEXT("%s_%s.png"), *SlotName, bMaster ? TEXT("master") : TEXT("field"));
	return FPaths::ProjectSavedDir() / TEXT("MapImages") / File;
}

bool USaveLoadSubsystem::DoesSaveExist(const FString& SlotName) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

bool USaveLoadSubsystem::SaveToSlot(const FString& SlotName, APawn* PlayerPawn, float TimeOfDay)
{
	ACartographerCharacter* Cart = Cast<ACartographerCharacter>(PlayerPawn);
	if (!Cart) { return false; }

	UCartographySaveGame* Save = Cast<UCartographySaveGame>(UGameplayStatics::CreateSaveGameObject(UCartographySaveGame::StaticClass()));
	if (!Save) { return false; }

	Save->SlotName = SlotName;
	Save->PlayerLocation = Cart->GetActorLocation();
	Save->PlayerRotation = Cart->GetActorRotation();
	Save->TimeOfDay = TimeOfDay;

	if (Cart->FieldMap)
	{
		Save->Ink = Cart->FieldMap->CurrentInk;
		const FString FieldPath  = GetSlotImagePath(SlotName, false);
		const FString MasterPath = GetSlotImagePath(SlotName, true);
		UMapDrawingLibrary::SaveRenderTargetToPNG(Cart->FieldMap->FieldMapRT, FieldPath);
		UMapDrawingLibrary::SaveRenderTargetToPNG(Cart->FieldMap->MasterMapRT, MasterPath);
		Save->FieldMapImageRelativePath  = FieldPath;
		Save->MasterMapImageRelativePath = MasterPath;
	}

	if (UWorld* World = Cart->GetWorld())
	{
		if (UWorldResponseSubsystem* Sub = World->GetSubsystem<UWorldResponseSubsystem>())
		{
			Sub->SaveStatesToArray(Save->CellStateBytes);
			Save->GridSize = Sub->GetGridSize();
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UProgressionSubsystem* Prog = GI->GetSubsystem<UProgressionSubsystem>())
		{
			Save->UnlockedTools = Prog->GetAllUnlocked();
		}
	}

	if (Cart->Notebook)
	{
		Save->Notebook.Reset();
		for (const FNotebookEntry& E : Cart->Notebook->Entries)
		{
			FSavedNotebookEntry S;
			S.Title = E.Title.ToString();
			S.Body  = E.Body.ToString();
			S.WorldLocation = E.WorldLocation;
			S.Category = E.Category;
			Save->Notebook.Add(S);
		}
	}

	return UGameplayStatics::SaveGameToSlot(Save, SlotName, 0);
}

bool USaveLoadSubsystem::LoadFromSlot(const FString& SlotName, APawn* PlayerPawn, float& OutTimeOfDay)
{
	if (!DoesSaveExist(SlotName)) { return false; }
	UCartographySaveGame* Save = Cast<UCartographySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!Save) { return false; }

	ACartographerCharacter* Cart = Cast<ACartographerCharacter>(PlayerPawn);
	if (!Cart) { return false; }

	Cart->SetActorLocationAndRotation(Save->PlayerLocation, Save->PlayerRotation);
	OutTimeOfDay = Save->TimeOfDay;

	if (Cart->FieldMap)
	{
		Cart->FieldMap->CurrentInk = Save->Ink;
		if (!Save->FieldMapImageRelativePath.IsEmpty())
		{
			UMapDrawingLibrary::LoadPNGToRenderTarget(Cart, Cart->FieldMap->FieldMapRT, Save->FieldMapImageRelativePath);
		}
		if (!Save->MasterMapImageRelativePath.IsEmpty())
		{
			UMapDrawingLibrary::LoadPNGToRenderTarget(Cart, Cart->FieldMap->MasterMapRT, Save->MasterMapImageRelativePath);
		}
	}

	if (UWorld* World = Cart->GetWorld())
	{
		if (UWorldResponseSubsystem* Sub = World->GetSubsystem<UWorldResponseSubsystem>())
		{
			Sub->LoadStatesFromArray(Save->CellStateBytes, Save->GridSize);
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UProgressionSubsystem* Prog = GI->GetSubsystem<UProgressionSubsystem>())
		{
			Prog->LoadUnlocks(Save->UnlockedTools);
		}
	}

	if (Cart->Notebook)
	{
		Cart->Notebook->Entries.Reset();
		for (const FSavedNotebookEntry& S : Save->Notebook)
		{
			Cart->Notebook->AddEntry(FText::FromString(S.Title), FText::FromString(S.Body), S.WorldLocation, S.Category);
		}
	}

	return true;
}
