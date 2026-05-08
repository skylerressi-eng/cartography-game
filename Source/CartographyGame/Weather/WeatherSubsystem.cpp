// Copyright (c) 2026 The Last Cartographer.

#include "Weather/WeatherSubsystem.h"

void UWeatherSubsystem::Forecast(EWeatherKind Kind, float Intensity, float WindSpeed, FVector WindDir)
{
	Target.Kind = Kind;
	Target.Intensity = FMath::Clamp(Intensity, 0.f, 1.f);
	Target.WindSpeed = FMath::Max(0.f, WindSpeed);
	Target.WindDirection = WindDir.GetSafeNormal2D();
	switch (Kind)
	{
	case EWeatherKind::Clear:     Target.Visibility = 1.0f;  break;
	case EWeatherKind::Cloudy:    Target.Visibility = 0.85f; break;
	case EWeatherKind::Fog:       Target.Visibility = 0.35f; break;
	case EWeatherKind::LightRain: Target.Visibility = 0.7f;  break;
	case EWeatherKind::HeavyRain: Target.Visibility = 0.45f; break;
	case EWeatherKind::Storm:     Target.Visibility = 0.25f; break;
	}
}

void UWeatherSubsystem::RandomiseForecast(int32 Seed)
{
	const FRandomStream R(Seed != 0 ? Seed : FMath::Rand());
	const EWeatherKind Kinds[] = { EWeatherKind::Clear, EWeatherKind::Cloudy, EWeatherKind::Fog,
	                                EWeatherKind::LightRain, EWeatherKind::HeavyRain };
	const EWeatherKind K = Kinds[R.RandRange(0, 4)];
	Forecast(K, R.FRandRange(0.3f, 1.f), R.FRandRange(0.f, 8.f),
	         FVector(R.FRandRange(-1.f, 1.f), R.FRandRange(-1.f, 1.f), 0.f));
}

void UWeatherSubsystem::ApplyAccuracyGlitch(float Glitch01)
{
	Target.Glitch = FMath::Clamp(Glitch01, 0.f, 1.f);
}

void UWeatherSubsystem::Tick(float DeltaSeconds)
{
	const float A = FMath::Clamp(BlendSpeed * DeltaSeconds, 0.f, 1.f);
	Current.Intensity      = FMath::Lerp(Current.Intensity,  Target.Intensity,  A);
	Current.WindSpeed      = FMath::Lerp(Current.WindSpeed,  Target.WindSpeed,  A);
	Current.WindDirection  = FMath::Lerp(Current.WindDirection, Target.WindDirection, A).GetSafeNormal();
	Current.Visibility     = FMath::Lerp(Current.Visibility, Target.Visibility, A);
	Current.Glitch         = FMath::Lerp(Current.Glitch,     Target.Glitch,     A);
	if (FMath::Abs((int32)Current.Kind - (int32)Target.Kind) > 0 && A > 0.5f) { Current.Kind = Target.Kind; }
	OnWeatherChanged.Broadcast(Current);
}
