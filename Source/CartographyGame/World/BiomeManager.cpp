// Copyright (c) 2026 The Last Cartographer.

#include "World/BiomeManager.h"

#include "Engine/DataTable.h"

void UBiomeManager::UpdateForLocation(FVector WorldLocation)
{
	if (!RegionsTable) { return; }
	FName Found = NAME_None;
	for (const auto& Pair : RegionsTable->GetRowMap())
	{
		const FRegionRow* R = (FRegionRow*)Pair.Value;
		if (!R) { continue; }
		const FVector2D Lo = R->OriginXY;
		const FVector2D Hi = R->OriginXY + R->SizeXY;
		if (WorldLocation.X >= Lo.X && WorldLocation.X <= Hi.X &&
		    WorldLocation.Y >= Lo.Y && WorldLocation.Y <= Hi.Y)
		{
			Found = R->RegionId;
			break;
		}
	}
	if (Found != CurrentRegionId)
	{
		CurrentRegionId = Found;
		OnRegionEntered.Broadcast(Found);
	}
}

FRegionRow UBiomeManager::GetRegionRow(FName RegionId) const
{
	if (RegionsTable)
	{
		for (const auto& Pair : RegionsTable->GetRowMap())
		{
			const FRegionRow* R = (FRegionRow*)Pair.Value;
			if (R && R->RegionId == RegionId) { return *R; }
		}
	}
	return FRegionRow();
}

FBiomeRow UBiomeManager::GetBiomeRow(FName BiomeId) const
{
	if (BiomesTable)
	{
		for (const auto& Pair : BiomesTable->GetRowMap())
		{
			const FBiomeRow* B = (FBiomeRow*)Pair.Value;
			if (B && B->BiomeId == BiomeId) { return *B; }
		}
	}
	return FBiomeRow();
}

FName UBiomeManager::GetBiomeForRegion(FName RegionId) const
{
	return GetRegionRow(RegionId).BiomeId;
}
