// Copyright (c) 2026 The Last Cartographer.

#include "Player/FootstepComponent.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "World/BiomeManager.h"

UFootstepComponent::UFootstepComponent() { PrimaryComponentTick.bCanEverTick = true; }

FName UFootstepComponent::ResolveSurfaceTag() const
{
	if (UWorld* World = GetWorld())
	{
		if (UBiomeManager* B = World->GetSubsystem<UBiomeManager>())
		{
			const FRegionRow R = B->GetRegionRow(B->CurrentRegionId);
			if (!R.BiomeId.IsNone()) { return R.BiomeId; }
		}
	}
	return TEXT("Default");
}

void UFootstepComponent::Step()
{
	const FName Tag = ResolveSurfaceTag();
	const FFootstepSet* Match = Sets.FindByPredicate([&](const FFootstepSet& S){ return S.SurfaceTag == Tag; });
	if (!Match || Match->Sounds.Num() == 0)
	{
		Match = Sets.FindByPredicate([&](const FFootstepSet& S){ return S.SurfaceTag == TEXT("Default"); });
	}
	if (!Match || Match->Sounds.Num() == 0) { return; }
	USoundBase* S = Match->Sounds[FMath::RandRange(0, Match->Sounds.Num() - 1)];
	if (S && GetOwner())
	{
		UGameplayStatics::PlaySoundAtLocation(this, S, GetOwner()->GetActorLocation(), Volume);
	}
}

void UFootstepComponent::TickComponent(float, ELevelTick, FActorComponentTickFunction*)
{
	AActor* Owner = GetOwner();
	if (!Owner) { return; }
	const FVector Loc = Owner->GetActorLocation();
	if (bHasLast)
	{
		Distance += FVector::Dist2D(Loc, LastLocation);
		const float Threshold = 1.f / FMath::Max(StridePerCm, 0.000001f);
		while (Distance >= Threshold)
		{
			Distance -= Threshold;
			Step();
		}
	}
	LastLocation = Loc;
	bHasLast = true;
}
