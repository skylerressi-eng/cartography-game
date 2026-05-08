// Copyright (c) 2026 The Last Cartographer.

#include "Progression/ProgressionSubsystem.h"

void UProgressionSubsystem::Unlock(FName ToolId)
{
	if (ToolId.IsNone()) { return; }
	bool bWasNew = false;
	UnlockedTools.Add(ToolId, &bWasNew);
	if (bWasNew)
	{
		OnToolUnlocked.Broadcast(ToolId);
	}
}

void UProgressionSubsystem::LoadUnlocks(const TArray<FName>& Tools)
{
	UnlockedTools.Reset();
	for (const FName& Id : Tools) { UnlockedTools.Add(Id); }
}

void UProgressionSubsystem::OnRegionInked(int32 RegionIndex)
{
	if (RegionIndex >= 1) { Unlock(TEXT("Compass")); }
	if (RegionIndex >= 3) { Unlock(TEXT("Elevation")); }
	if (RegionIndex >= 5) { Unlock(TEXT("Watercolor")); }
	if (RegionIndex >= 8) { Unlock(TEXT("FastTravel")); }
}
