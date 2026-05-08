// Copyright (c) 2026 The Last Cartographer.

#include "FastTravel/FastTravelSubsystem.h"

#include "FastTravel/FastTravelMarker.h"
#include "GameFramework/Pawn.h"

void UFastTravelSubsystem::RegisterMarker(AFastTravelMarker* Marker)
{
	if (!Marker) { return; }
	Markers.AddUnique(Marker);
	if (Marker->bDiscovered) { OnDiscovered.Broadcast(Marker); }
}

TArray<AFastTravelMarker*> UFastTravelSubsystem::GetAvailable() const
{
	TArray<AFastTravelMarker*> Out;
	for (const TWeakObjectPtr<AFastTravelMarker>& W : Markers)
	{
		if (AFastTravelMarker* M = W.Get())
		{
			M->RefreshAvailability();
			if (M->bAvailable) { Out.Add(M); }
		}
	}
	return Out;
}

bool UFastTravelSubsystem::TravelTo(APawn* Pawn, FName MarkerId)
{
	if (!Pawn) { return false; }
	for (const TWeakObjectPtr<AFastTravelMarker>& W : Markers)
	{
		AFastTravelMarker* M = W.Get();
		if (!M || M->MarkerId != MarkerId) { continue; }
		M->RefreshAvailability();
		if (!M->bAvailable) { return false; }
		Pawn->SetActorLocation(M->GetActorLocation() + FVector(0, 0, 100.f));
		return true;
	}
	return false;
}
