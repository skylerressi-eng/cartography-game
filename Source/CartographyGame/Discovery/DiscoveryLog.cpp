// Copyright (c) 2026 The Last Cartographer.

#include "Discovery/DiscoveryLog.h"

void UDiscoveryLog::Record(FName Id, FText Name, FName Category, FVector WorldLocation)
{
	if (HasDiscovered(Id)) { return; }
	FDiscoveryEntry E;
	E.Id = Id; E.Name = Name; E.Category = Category;
	E.Timestamp = FDateTime::UtcNow();
	E.WorldLocation = WorldLocation;
	Entries.Add(E);
	OnDiscovery.Broadcast(E);
}

bool UDiscoveryLog::HasDiscovered(FName Id) const
{
	return Entries.ContainsByPredicate([&](const FDiscoveryEntry& E){ return E.Id == Id; });
}

int32 UDiscoveryLog::CountByCategory(FName Category) const
{
	int32 N = 0;
	for (const FDiscoveryEntry& E : Entries) { if (E.Category == Category) { ++N; } }
	return N;
}
