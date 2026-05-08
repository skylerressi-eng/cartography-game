// Copyright (c) 2026 The Last Cartographer.

#include "Time/TimeOfDaySubsystem.h"

#include "Engine/DirectionalLight.h"
#include "EngineUtils.h"
#include "Components/LightComponent.h"

void UTimeOfDaySubsystem::SetHour(float NewHour)
{
	CurrentHour = FMath::Fmod(FMath::Max(0.f, NewHour), 24.f);
	ApplySunRotation();
	const int32 Bucket = FMath::FloorToInt(CurrentHour);
	if (Bucket != LastHourBucket) { LastHourBucket = Bucket; OnHourChanged.Broadcast(Bucket); }
	const FName P = GetPhase();
	if (P != LastPhase) { LastPhase = P; OnPhaseChanged.Broadcast(P); }
}

void UTimeOfDaySubsystem::SkipToHour(float NewHour) { SetHour(NewHour); }

void UTimeOfDaySubsystem::SleepUntilMorning() { SetHour(7.f); }

FName UTimeOfDaySubsystem::GetPhase() const
{
	if (CurrentHour < 5.f)  return TEXT("Night");
	if (CurrentHour < 7.f)  return TEXT("Dawn");
	if (CurrentHour < 11.f) return TEXT("Morning");
	if (CurrentHour < 14.f) return TEXT("Noon");
	if (CurrentHour < 17.f) return TEXT("Afternoon");
	if (CurrentHour < 20.f) return TEXT("Dusk");
	return TEXT("Night");
}

float UTimeOfDaySubsystem::GetSunPitchDegrees() const
{
	// 6h -> sunrise (-5°), 12h -> noon (-80°), 18h -> sunset (-5°)
	const float T = (CurrentHour - 6.f) / 12.f; // 0..1 over daylight hours
	if (T < 0.f || T > 1.f) { return 5.f; }     // below horizon at night
	const float Pitch = -FMath::Sin(T * PI) * 80.f - 5.f;
	return Pitch;
}

void UTimeOfDaySubsystem::ApplySunRotation()
{
	UWorld* World = GetWorld();
	if (!World) { return; }
	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		ADirectionalLight* Sun = *It;
		if (Sun && (Sun->ActorHasTag(TEXT("SunLight")) || Sun->Tags.Num() == 0))
		{
			Sun->SetActorRotation(FRotator(GetSunPitchDegrees(), CurrentHour * 15.f, 0.f));
			break;
		}
	}
}

void UTimeOfDaySubsystem::Tick(float DeltaSeconds)
{
	const float Prev = CurrentHour;
	CurrentHour = FMath::Fmod(CurrentHour + DeltaSeconds * HoursPerSecond * 24.f, 24.f);
	if (FMath::FloorToInt(Prev) != FMath::FloorToInt(CurrentHour))
	{
		const int32 Bucket = FMath::FloorToInt(CurrentHour);
		LastHourBucket = Bucket;
		OnHourChanged.Broadcast(Bucket);
	}
	const FName P = GetPhase();
	if (P != LastPhase) { LastPhase = P; OnPhaseChanged.Broadcast(P); }
	ApplySunRotation();
}
