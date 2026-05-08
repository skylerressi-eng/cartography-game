// Copyright (c) 2026 The Last Cartographer.

#include "Inking/InkingDesk.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Map/FieldMapComponent.h"
#include "Player/CartographerCharacter.h"

AInkingDesk::AInkingDesk()
{
	PrimaryActorTick.bCanEverTick = false;

	DeskMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeskMesh"));
	RootComponent = DeskMesh;
	DeskMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractVolume"));
	InteractVolume->SetupAttachment(RootComponent);
	InteractVolume->SetBoxExtent(FVector(120.f, 100.f, 100.f));
	InteractVolume->SetRelativeLocation(FVector(80.f, 0.f, 50.f));
	InteractVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

bool AInkingDesk::IsPawnInRange(APawn* Pawn) const
{
	if (!Pawn || !InteractVolume) { return false; }
	TArray<AActor*> Overlapping;
	InteractVolume->GetOverlappingActors(Overlapping, APawn::StaticClass());
	return Overlapping.Contains(Pawn);
}

bool AInkingDesk::TryBeginInking(APawn* Pawn)
{
	if (!IsPawnInRange(Pawn)) { return false; }
	OnInkingBegan.Broadcast(Pawn);
	return true;
}

void AInkingDesk::EndInking(APawn* Pawn)
{
	OnInkingEnded.Broadcast(Pawn);
}

void AInkingDesk::RefillInk(APawn* Pawn) const
{
	ACartographerCharacter* Cart = Cast<ACartographerCharacter>(Pawn);
	if (Cart && Cart->FieldMap)
	{
		Cart->FieldMap->RefillInkToMax();
	}
}
