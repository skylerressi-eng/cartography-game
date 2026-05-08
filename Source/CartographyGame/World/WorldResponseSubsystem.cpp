// Copyright (c) 2026 The Last Cartographer.

#include "World/WorldResponseSubsystem.h"

void UWorldResponseSubsystem::EnsureSize()
{
	const int32 Count = GridSize * GridSize;
	if (CellStates.Num() != Count)
	{
		CellStates.Init(ECellAccuracy::Unmapped, Count);
	}
}

void UWorldResponseSubsystem::SetGridConfig(FVector2D InOrigin, FVector2D InSize, int32 InGridSize)
{
	OriginXY = InOrigin;
	SizeXY   = InSize;
	GridSize = FMath::Max(1, InGridSize);
	EnsureSize();
}

void UWorldResponseSubsystem::ApplyComparisonResults(const TArray<FCellComparisonResult>& Results)
{
	EnsureSize();
	for (const FCellComparisonResult& R : Results)
	{
		const int32 Idx = R.Cell.Y * GridSize + R.Cell.X;
		if (!CellStates.IsValidIndex(Idx)) { continue; }
		const ECellAccuracy Old = CellStates[Idx];
		if (Old != R.Accuracy)
		{
			CellStates[Idx] = R.Accuracy;
			OnCellAccuracyChanged.Broadcast(R.Cell, R.Accuracy);
		}
	}
}

ECellAccuracy UWorldResponseSubsystem::GetAccuracy(FIntPoint Cell) const
{
	if (Cell.X < 0 || Cell.Y < 0 || Cell.X >= GridSize || Cell.Y >= GridSize) { return ECellAccuracy::Unmapped; }
	const int32 Idx = Cell.Y * GridSize + Cell.X;
	return CellStates.IsValidIndex(Idx) ? CellStates[Idx] : ECellAccuracy::Unmapped;
}

FIntPoint UWorldResponseSubsystem::WorldToCell(FVector WorldLocation) const
{
	const float U = (WorldLocation.X - OriginXY.X) / FMath::Max(SizeXY.X, 1.f);
	const float V = (WorldLocation.Y - OriginXY.Y) / FMath::Max(SizeXY.Y, 1.f);
	return FIntPoint(
		FMath::Clamp(FMath::FloorToInt(U * GridSize), 0, GridSize - 1),
		FMath::Clamp(FMath::FloorToInt(V * GridSize), 0, GridSize - 1));
}

ECellAccuracy UWorldResponseSubsystem::GetAccuracyAtWorld(FVector WorldLocation) const
{
	return GetAccuracy(WorldToCell(WorldLocation));
}

void UWorldResponseSubsystem::SaveStatesToArray(TArray<uint8>& OutBytes) const
{
	OutBytes.Reset(CellStates.Num());
	for (ECellAccuracy A : CellStates) { OutBytes.Add((uint8)A); }
}

void UWorldResponseSubsystem::LoadStatesFromArray(const TArray<uint8>& Bytes, int32 InGridSize)
{
	GridSize = FMath::Max(1, InGridSize);
	const int32 Count = GridSize * GridSize;
	CellStates.SetNum(Count);
	for (int32 i = 0; i < Count; ++i)
	{
		CellStates[i] = (i < Bytes.Num()) ? (ECellAccuracy)Bytes[i] : ECellAccuracy::Unmapped;
	}
	for (int32 cy = 0; cy < GridSize; ++cy)
	{
		for (int32 cx = 0; cx < GridSize; ++cx)
		{
			OnCellAccuracyChanged.Broadcast(FIntPoint(cx, cy), CellStates[cy * GridSize + cx]);
		}
	}
}
