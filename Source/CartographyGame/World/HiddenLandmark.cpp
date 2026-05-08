// Copyright (c) 2026 The Last Cartographer.

#include "World/HiddenLandmark.h"

#include "Components/StaticMeshComponent.h"
#include "World/WorldResponseSubsystem.h"

AHiddenLandmark::AHiddenLandmark()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AHiddenLandmark::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (UWorldResponseSubsystem* Sub = World->GetSubsystem<UWorldResponseSubsystem>())
		{
			MyCell = Sub->WorldToCell(GetActorLocation());
			Sub->OnCellAccuracyChanged.AddDynamic(this, &AHiddenLandmark::HandleCellChanged);
			ApplyVisibility(Sub->GetAccuracy(MyCell));
		}
	}
}

void AHiddenLandmark::HandleCellChanged(FIntPoint Cell, ECellAccuracy NewAccuracy)
{
	if (Cell == MyCell)
	{
		ApplyVisibility(NewAccuracy);
	}
}

void AHiddenLandmark::RefreshFromWorldResponse()
{
	if (UWorld* World = GetWorld())
	{
		if (UWorldResponseSubsystem* Sub = World->GetSubsystem<UWorldResponseSubsystem>())
		{
			MyCell = Sub->WorldToCell(GetActorLocation());
			ApplyVisibility(Sub->GetAccuracy(MyCell));
		}
	}
}

void AHiddenLandmark::ApplyVisibility(ECellAccuracy Accuracy)
{
	const bool bShouldShow = (uint8)Accuracy >= (uint8)MinAccuracyToReveal;
	SetActorHiddenInGame(!bShouldShow);
	SetActorEnableCollision(bShouldShow);
}
