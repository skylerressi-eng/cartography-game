// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveLoadSubsystem.generated.h"

class APawn;

UCLASS()
class CARTOGRAPHYGAME_API USaveLoadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Save")
	bool SaveToSlot(const FString& SlotName, APawn* PlayerPawn, float TimeOfDay);

	UFUNCTION(BlueprintCallable, Category="Save")
	bool LoadFromSlot(const FString& SlotName, APawn* PlayerPawn, float& OutTimeOfDay);

	UFUNCTION(BlueprintCallable, Category="Save")
	bool DoesSaveExist(const FString& SlotName) const;

	UFUNCTION(BlueprintCallable, Category="Save")
	FString GetSlotImagePath(const FString& SlotName, bool bMaster) const;
};
