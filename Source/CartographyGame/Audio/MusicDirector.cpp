// Copyright (c) 2026 The Last Cartographer.

#include "Audio/MusicDirector.h"

#include "Audio/AudioMoodComponent.h"
#include "GroundTruth/MapComparisonComponent.h"
#include "Time/TimeOfDaySubsystem.h"
#include "Weather/WeatherSubsystem.h"
#include "World/WorldResponseSubsystem.h"

void UMusicDirector::Tick(float DeltaSeconds)
{
	if (!Mixer.IsValid()) { return; }

	UWorld* World = GetWorld();
	UTimeOfDaySubsystem* Tod = World ? World->GetSubsystem<UTimeOfDaySubsystem>() : nullptr;
	UWeatherSubsystem*   Wx  = World ? World->GetSubsystem<UWeatherSubsystem>()   : nullptr;
	UWorldResponseSubsystem* Wr = World ? World->GetSubsystem<UWorldResponseSubsystem>() : nullptr;

	float Calm = 0.6f, Drone = 0.f, Wonder = 0.f, Storm = 0.f, Sleep = 0.f;

	if (Tod)
	{
		const FName Phase = Tod->GetPhase();
		if (Phase == TEXT("Night")) { Sleep = 0.4f; Drone = 0.3f; Calm = 0.3f; }
		if (Phase == TEXT("Dawn") || Phase == TEXT("Dusk")) { Wonder = 0.4f; Calm = 0.6f; }
	}
	if (Wx && (Wx->Current.Kind == EWeatherKind::Storm || Wx->Current.Kind == EWeatherKind::HeavyRain))
	{
		Storm = FMath::Max(Storm, Wx->Current.Intensity);
		Calm = FMath::Min(Calm, 0.2f);
	}
	if (Wr)
	{
		// Could query player location-based accuracy here.
	}

	DiscoveryStingerEnergy = FMath::FInterpTo(DiscoveryStingerEnergy, 0.f, DeltaSeconds, 0.5f);
	Wonder = FMath::Max(Wonder, DiscoveryStingerEnergy);

	Mixer->SetLayerTarget(TEXT("Calm"),   Calm);
	Mixer->SetLayerTarget(TEXT("Drone"),  Drone);
	Mixer->SetLayerTarget(TEXT("Wonder"), Wonder);
	Mixer->SetLayerTarget(TEXT("Storm"),  Storm);
	Mixer->SetLayerTarget(TEXT("Sleep"),  Sleep);
}
