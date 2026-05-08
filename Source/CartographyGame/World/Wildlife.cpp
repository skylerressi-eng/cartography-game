// Copyright (c) 2026 The Last Cartographer.

#include "World/Wildlife.h"

#include "Components/SkeletalMeshComponent.h"

AWildlife::AWildlife()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWildlife::BeginPlay()
{
	Super::BeginPlay();
	HomeLocation = GetActorLocation();
	CurrentGoal = HomeLocation;
	Replan();
}

void AWildlife::Replan()
{
	const FVector2D Disc = FMath::RandPointInCircle(LeashRadius);
	CurrentGoal = HomeLocation + FVector(Disc.X, Disc.Y, HomeAltitudeOffset);
	TimeUntilReplan = ReplanInterval + FMath::FRandRange(-1.f, 1.f);
}

void AWildlife::Tick(float Dt)
{
	Super::Tick(Dt);
	const FVector Loc = GetActorLocation();
	const FVector ToGoal = CurrentGoal - Loc;
	const float Dist = ToGoal.Size();
	if (Dist < 50.f) { TimeUntilReplan = 0.f; }
	TimeUntilReplan -= Dt;
	if (TimeUntilReplan <= 0.f) { Replan(); }

	const FVector Dir = ToGoal.GetSafeNormal();
	SetActorLocation(Loc + Dir * Speed * Dt, true);
	if (!Dir.IsNearlyZero()) { SetActorRotation(FMath::RInterpTo(GetActorRotation(), Dir.Rotation(), Dt, 4.f)); }
}
