// Copyright (c) 2026 The Last Cartographer.

#include "FastTravel/FastTravelMarker.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FastTravel/FastTravelSubsystem.h"
#include "GameFramework/Pawn.h"
#include "World/WorldResponseSubsystem.h"

AFastTravelMarker::AFastTravelMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	ActivationSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	ActivationSphere->SetupAttachment(RootComponent);
	ActivationSphere->InitSphereRadius(200.f);
	ActivationSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AFastTravelMarker::BeginPlay()
{
	Super::BeginPlay();
	ActivationSphere->OnComponentBeginOverlap.AddDynamic(this, &AFastTravelMarker::HandleOverlap);
	if (UFastTravelSubsystem* Sub = GetWorld()->GetSubsystem<UFastTravelSubsystem>())
	{
		Sub->RegisterMarker(this);
	}
	RefreshAvailability();
}

void AFastTravelMarker::HandleOverlap(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (Cast<APawn>(Other)) { Discover(); }
}

void AFastTravelMarker::Discover()
{
	if (bDiscovered) { return; }
	bDiscovered = true;
	RefreshAvailability();
}

void AFastTravelMarker::RefreshAvailability()
{
	if (!bDiscovered) { bAvailable = false; return; }
	if (!bRequiresAccurateMapping) { bAvailable = true; return; }
	if (UWorldResponseSubsystem* Wr = GetWorld()->GetSubsystem<UWorldResponseSubsystem>())
	{
		bAvailable = Wr->GetAccuracyAtWorld(GetActorLocation()) == ECellAccuracy::AccuratelyMapped;
		return;
	}
	bAvailable = false;
}
