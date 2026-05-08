// Copyright (c) 2026 The Last Cartographer.

#include "Core/MPCDriver.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Time/TimeOfDaySubsystem.h"
#include "Weather/WeatherSubsystem.h"
#include "World/BiomeManager.h"
#include "World/WorldResponseSubsystem.h"

void UMPCDriver::Tick(float Dt)
{
	if (!Collection) { return; }
	Accum += Dt;
	if (Accum < UpdateInterval) { return; }
	Accum = 0.f;

	UWorld* World = GetWorld();
	if (!World) { return; }

	if (UTimeOfDaySubsystem* T = World->GetSubsystem<UTimeOfDaySubsystem>())
	{
		UKismetMaterialLibrary::SetScalarParameterValue(World, Collection, TEXT("TimeOfDay"), T->CurrentHour);
	}
	if (UWeatherSubsystem* Wx = World->GetSubsystem<UWeatherSubsystem>())
	{
		UKismetMaterialLibrary::SetScalarParameterValue(World, Collection, TEXT("WeatherIntensity"), Wx->Current.Intensity);
		UKismetMaterialLibrary::SetScalarParameterValue(World, Collection, TEXT("Fog"),              1.f - Wx->Current.Visibility);
		UKismetMaterialLibrary::SetScalarParameterValue(World, Collection, TEXT("AccuracyGlitch"),   Wx->Current.Glitch);
		UKismetMaterialLibrary::SetVectorParameterValue(World, Collection, TEXT("WindDirection"),
			FLinearColor(Wx->Current.WindDirection.X, Wx->Current.WindDirection.Y, Wx->Current.WindDirection.Z, Wx->Current.WindSpeed));
	}
	if (UBiomeManager* B = World->GetSubsystem<UBiomeManager>())
	{
		const FBiomeRow Row = B->GetBiomeRow(B->GetBiomeForRegion(B->CurrentRegionId));
		UKismetMaterialLibrary::SetVectorParameterValue(World, Collection, TEXT("BiomeTint"), Row.FogTint);
	}
}
