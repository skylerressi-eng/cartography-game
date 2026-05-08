// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GroundTruth/MapComparisonComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldResponseSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCellAccuracyChanged, FIntPoint, Cell, ECellAccuracy, NewAccuracy);

/**
 * Owns the per-cell accuracy state for the island. HiddenLandmarks and
 * weather/fast-travel systems subscribe to OnCellAccuracyChanged, or query
 * GetAccuracyAtWorld() when they need to make a decision.
 *
 * Set the world bounds in BP_GameMode::BeginPlay (CallSetGridConfig) so
 * world-XY positions can be projected to cell indices.
 */
UCLASS()
class CARTOGRAPHYGAME_API UWorldResponseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="WorldResponse")
	FOnCellAccuracyChanged OnCellAccuracyChanged;

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	void SetGridConfig(FVector2D OriginXY, FVector2D SizeXY, int32 InGridSize);

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	void ApplyComparisonResults(const TArray<FCellComparisonResult>& Results);

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	ECellAccuracy GetAccuracy(FIntPoint Cell) const;

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	ECellAccuracy GetAccuracyAtWorld(FVector WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	FIntPoint WorldToCell(FVector WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	int32 GetGridSize() const { return GridSize; }

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	const TArray<ECellAccuracy>& GetAllStates() const { return CellStates; }

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	void LoadStatesFromArray(const TArray<uint8>& Bytes, int32 InGridSize);

	UFUNCTION(BlueprintCallable, Category="WorldResponse")
	void SaveStatesToArray(TArray<uint8>& OutBytes) const;

private:
	UPROPERTY() FVector2D OriginXY = FVector2D::ZeroVector;
	UPROPERTY() FVector2D SizeXY   = FVector2D(100000.f, 100000.f);
	UPROPERTY() int32 GridSize     = 32;
	UPROPERTY() TArray<ECellAccuracy> CellStates;

	void EnsureSize();
};
