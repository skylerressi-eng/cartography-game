// Copyright (c) 2026 The Last Cartographer.

#include "World/PirateIslandWorld.h"

#include "Components/SceneComponent.h"
#include "World/BuildingHider.h"

APirateIslandWorld::APirateIslandWorld()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Hider = CreateDefaultSubobject<UBuildingHiderComponent>(TEXT("Hider"));
}

FBox APirateIslandWorld::ComputeWorldBounds() const
{
	FBox Bounds(EForceInit::ForceInit);
	TArray<UActorComponent*> Comps;
	GetComponents(Comps);
	for (UActorComponent* C : Comps)
	{
		USceneComponent* S = Cast<USceneComponent>(C);
		if (S) { Bounds += S->Bounds.GetBox(); }
	}
	return Bounds;
}

FVector2D APirateIslandWorld::GetWorldOriginXY() const
{
	const FVector L = GetActorLocation();
	return FVector2D(L.X - WorldExtentXY.X * 0.5f, L.Y - WorldExtentXY.Y * 0.5f);
}
