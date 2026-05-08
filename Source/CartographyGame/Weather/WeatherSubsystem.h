// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WeatherSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWeatherKind : uint8 { Clear, Cloudy, Fog, LightRain, HeavyRain, Storm };

USTRUCT(BlueprintType)
struct FWeatherState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite) EWeatherKind Kind = EWeatherKind::Clear;
	UPROPERTY(BlueprintReadWrite) float Intensity = 0.f;     // 0..1
	UPROPERTY(BlueprintReadWrite) float WindSpeed = 0.f;     // m/s
	UPROPERTY(BlueprintReadWrite) FVector WindDirection = FVector::ForwardVector;
	UPROPERTY(BlueprintReadWrite) float Visibility = 1.f;    // 0..1
	UPROPERTY(BlueprintReadWrite) float Glitch = 0.f;        // 0..1, used by inaccurate cells
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, const FWeatherState&, NewState);

/**
 * Drives weather as a slow blend of forecast targets. Inaccurate map cells
 * inject "Glitch" — small unpredictable bursts that look subtly wrong.
 */
UCLASS()
class CARTOGRAPHYGAME_API UWeatherSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Weather") FOnWeatherChanged OnWeatherChanged;
	UPROPERTY(BlueprintReadOnly,    Category="Weather") FWeatherState Current;
	UPROPERTY(BlueprintReadOnly,    Category="Weather") FWeatherState Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather") float BlendSpeed = 0.05f;

	UFUNCTION(BlueprintCallable, Category="Weather")
	void Forecast(EWeatherKind Kind, float Intensity, float WindSpeed, FVector WindDir);

	UFUNCTION(BlueprintCallable, Category="Weather")
	void RandomiseForecast(int32 Seed = 0);

	UFUNCTION(BlueprintCallable, Category="Weather")
	void ApplyAccuracyGlitch(float Glitch01);

	virtual void Tick(float DeltaSeconds) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UWeatherSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }
};
