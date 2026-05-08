// Copyright (c) 2026 The Last Cartographer.

#include "Discovery/DiscoveryHintSubsystem.h"

#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "World/HiddenLandmark.h"
#include "World/WorldResponseSubsystem.h"

void UDiscoveryHintSubsystem::Tick(float Dt)
{
	UWorld* World = GetWorld();
	if (!World) { return; }
	Cooldown = FMath::Max(0.f, Cooldown - Dt);
	if (Cooldown > 0.f || !WhisperSound) { return; }

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!Pawn) { return; }
	const FVector Loc = Pawn->GetActorLocation();

	UWorldResponseSubsystem* Wr = World->GetSubsystem<UWorldResponseSubsystem>();
	for (TActorIterator<AHiddenLandmark> It(World); It; ++It)
	{
		AHiddenLandmark* L = *It;
		if (!L || !L->IsHidden()) { continue; }
		if (FVector::Dist(L->GetActorLocation(), Loc) > ProbeRadius) { continue; }
		if (Wr)
		{
			const ECellAccuracy A = Wr->GetAccuracyAtWorld(L->GetActorLocation());
			if (A == ECellAccuracy::AccuratelyMapped) { continue; }
		}
		UGameplayStatics::PlaySound2D(World, WhisperSound, Volume);
		Cooldown = CooldownSeconds;
		return;
	}
}
