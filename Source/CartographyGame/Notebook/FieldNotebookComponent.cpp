// Copyright (c) 2026 The Last Cartographer.

#include "Notebook/FieldNotebookComponent.h"

#include "GameFramework/Actor.h"
#include "Notebook/MapPin.h"

UFieldNotebookComponent::UFieldNotebookComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UFieldNotebookComponent::AddEntry(FText Title, FText Body, FVector WorldLocation, FName Category)
{
	FNotebookEntry Entry;
	Entry.Title = MoveTemp(Title);
	Entry.Body = MoveTemp(Body);
	Entry.WorldLocation = WorldLocation;
	Entry.Category = Category;

	if (UWorld* World = GetWorld())
	{
		if (PinClass)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AMapPin* Pin = World->SpawnActor<AMapPin>(PinClass, WorldLocation, FRotator::ZeroRotator, Params);
			if (Pin)
			{
				Pin->Note = Entry.Body;
				Pin->Category = Category;
				Entry.SpawnedPin = Pin;
			}
		}
	}

	const int32 Index = Entries.Add(MoveTemp(Entry));
	OnNotebookChanged.Broadcast(Index);
	return Index;
}

int32 UFieldNotebookComponent::PinAtPlayerLocation(FText Title, FText Body, FName Category)
{
	const FVector Location = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
	return AddEntry(MoveTemp(Title), MoveTemp(Body), Location, Category);
}

void UFieldNotebookComponent::RemoveEntry(int32 Index)
{
	if (!Entries.IsValidIndex(Index)) { return; }
	if (Entries[Index].SpawnedPin.IsValid())
	{
		Entries[Index].SpawnedPin->Destroy();
	}
	Entries.RemoveAt(Index);
	OnNotebookChanged.Broadcast(Index);
}

FVector2D UFieldNotebookComponent::WorldToMapUV(FVector WorldLocation) const
{
	const float U = (WorldLocation.X - WorldOriginXY.X) / FMath::Max(WorldSizeXY.X, 1.f);
	const float V = (WorldLocation.Y - WorldOriginXY.Y) / FMath::Max(WorldSizeXY.Y, 1.f);
	return FVector2D(FMath::Clamp(U, 0.f, 1.f), FMath::Clamp(V, 0.f, 1.f));
}
