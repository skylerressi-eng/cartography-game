// Copyright (c) 2026 The Last Cartographer.

#include "World/TrailComponent.h"

#include "GameFramework/Actor.h"

UTrailComponent::UTrailComponent() { PrimaryComponentTick.bCanEverTick = true; }

void UTrailComponent::ResetTrail() { Samples.Reset(); }

void UTrailComponent::TickComponent(float, ELevelTick, FActorComponentTickFunction*)
{
	AActor* Owner = GetOwner();
	if (!Owner) { return; }
	const FVector Loc = Owner->GetActorLocation();
	if (Samples.Num() == 0 || FVector::DistSquared(Samples.Last(), Loc) >= FMath::Square(MinSampleDistance))
	{
		Samples.Add(Loc);
		if (Samples.Num() > MaxSamples) { Samples.RemoveAt(0); }
	}
}

void UTrailComponent::SerializeToBytes(TArray<uint8>& Out) const
{
	Out.SetNumUninitialized(Samples.Num() * sizeof(FVector));
	if (Samples.Num() > 0)
	{
		FMemory::Memcpy(Out.GetData(), Samples.GetData(), Out.Num());
	}
}

void UTrailComponent::DeserializeFromBytes(const TArray<uint8>& In)
{
	const int32 Count = In.Num() / sizeof(FVector);
	Samples.SetNumUninitialized(Count);
	if (Count > 0)
	{
		FMemory::Memcpy(Samples.GetData(), In.GetData(), Count * sizeof(FVector));
	}
}
