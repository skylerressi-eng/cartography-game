// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeOfDaySubsystem.generated.h"

class ADirectionalLight;
class ASkyAtmosphere;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, int32, NewHour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, FName, NewPhase);

/**
 * Drives a 24h day with fast forward at sleep. Phases: "Dawn", "Morning",
 * "Noon", "Afternoon", "Dusk", "Night". The sun light rotation is computed
 * from CurrentHour - the world subsystem looks up an actor tagged
 * "SunLight" once at startup and rotates it every Tick.
 */
UCLASS()
class CARTOGRAPHYGAME_API UTimeOfDaySubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Time") FOnHourChanged OnHourChanged;
	UPROPERTY(BlueprintAssignable, Category="Time") FOnPhaseChanged OnPhaseChanged;

	/** Hours per real-world second. Default = 1 in-game hour per 60s real. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time") float HoursPerSecond = 1.f / 60.f;

	UPROPERTY(BlueprintReadOnly, Category="Time") float CurrentHour = 8.f;

	UFUNCTION(BlueprintCallable, Category="Time") void SetHour(float NewHour);
	UFUNCTION(BlueprintCallable, Category="Time") void SkipToHour(float NewHour);
	UFUNCTION(BlueprintCallable, Category="Time") void SleepUntilMorning();
	UFUNCTION(BlueprintPure,    Category="Time") FName GetPhase() const;
	UFUNCTION(BlueprintPure,    Category="Time") float GetSunPitchDegrees() const;
	UFUNCTION(BlueprintPure,    Category="Time") bool IsNight() const { return CurrentHour < 6.f || CurrentHour >= 20.f; }

	// UTickableWorldSubsystem
	virtual void Tick(float DeltaSeconds) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeOfDaySubsystem, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }

private:
	int32 LastHourBucket = -1;
	FName LastPhase = NAME_None;
	void ApplySunRotation();
};
