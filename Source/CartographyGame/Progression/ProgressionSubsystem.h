// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ProgressionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolUnlocked, FName, ToolId);

/**
 * Tools/abilities the player accumulates over the run. Tool ids are FNames
 * so designers can add new ones via data tables without recompiling. Common
 * ids the rest of the systems already check for:
 *   "Compass"        — small north arrow on the field map
 *   "Watercolor"     — colour brush on inked map
 *   "Elevation"      — elevation marker tool on field map
 *   "FastTravel"     — enables fast travel pins on accurate cells
 */
UCLASS()
class CARTOGRAPHYGAME_API UProgressionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Progression")
	FOnToolUnlocked OnToolUnlocked;

	UFUNCTION(BlueprintCallable, Category="Progression")
	void Unlock(FName ToolId);

	UFUNCTION(BlueprintCallable, Category="Progression")
	bool IsUnlocked(FName ToolId) const { return UnlockedTools.Contains(ToolId); }

	UFUNCTION(BlueprintCallable, Category="Progression")
	TArray<FName> GetAllUnlocked() const { return UnlockedTools.Array(); }

	UFUNCTION(BlueprintCallable, Category="Progression")
	void LoadUnlocks(const TArray<FName>& Tools);

	UFUNCTION(BlueprintCallable, Category="Progression")
	void OnRegionInked(int32 RegionIndex);

private:
	UPROPERTY() TSet<FName> UnlockedTools;
};
