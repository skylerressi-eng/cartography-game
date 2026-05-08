// Copyright (c) 2026 The Last Cartographer.

#include "World/WorldHider.h"

#include "Components/BillboardComponent.h"
#include "World/BuildingHider.h"

AWorldHider::AWorldHider()
{
	PrimaryActorTick.bCanEverTick = false;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;

	Hider = CreateDefaultSubobject<UBuildingHiderComponent>(TEXT("Hider"));
}
