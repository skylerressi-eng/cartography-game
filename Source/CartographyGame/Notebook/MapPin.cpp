// Copyright (c) 2026 The Last Cartographer.

#include "Notebook/MapPin.h"

#include "Components/StaticMeshComponent.h"

AMapPin::AMapPin()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetMobility(EComponentMobility::Movable);
}
