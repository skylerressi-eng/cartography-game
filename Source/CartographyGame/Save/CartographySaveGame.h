// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Notebook/FieldNotebookComponent.h"
#include "CartographySaveGame.generated.h"

USTRUCT()
struct FSavedNotebookEntry
{
	GENERATED_BODY()

	UPROPERTY() FString Title;
	UPROPERTY() FString Body;
	UPROPERTY() FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY() FName Category = TEXT("General");
};

UCLASS()
class CARTOGRAPHYGAME_API UCartographySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY() FString SlotName = TEXT("Default");

	UPROPERTY() FVector PlayerLocation = FVector::ZeroVector;
	UPROPERTY() FRotator PlayerRotation = FRotator::ZeroRotator;
	UPROPERTY() float TimeOfDay = 12.f;        // hours, 0..24
	UPROPERTY() float Ink = 100.f;

	UPROPERTY() TArray<uint8> CellStateBytes;  // ECellAccuracy values
	UPROPERTY() int32 GridSize = 32;

	UPROPERTY() TArray<FName> UnlockedTools;
	UPROPERTY() TArray<FSavedNotebookEntry> Notebook;

	/** Side-files written next to the .sav: <Slot>_field.png and <Slot>_master.png. */
	UPROPERTY() FString FieldMapImageRelativePath;
	UPROPERTY() FString MasterMapImageRelativePath;
};
