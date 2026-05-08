// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "World/RegionData.h"
#include "BiomeManager.generated.h"

class UDataTable;
class UAudioMoodComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegionEntered, FName, RegionId);

/**
 * Tracks which region/biome the player is in, swaps audio + tint cues at
 * region borders. Drive from the player tick by calling UpdateForLocation.
 */
UCLASS()
class CARTOGRAPHYGAME_API UBiomeManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome") TObjectPtr<UDataTable> RegionsTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome") TObjectPtr<UDataTable> BiomesTable;

	UPROPERTY(BlueprintAssignable, Category="Biome") FOnRegionEntered OnRegionEntered;
	UPROPERTY(BlueprintReadOnly,    Category="Biome") FName CurrentRegionId;

	UFUNCTION(BlueprintCallable, Category="Biome") void UpdateForLocation(FVector WorldLocation);
	UFUNCTION(BlueprintCallable, Category="Biome") FRegionRow GetRegionRow(FName RegionId) const;
	UFUNCTION(BlueprintCallable, Category="Biome") FBiomeRow  GetBiomeRow(FName BiomeId) const;
	UFUNCTION(BlueprintPure,    Category="Biome") FName GetBiomeForRegion(FName RegionId) const;
};
