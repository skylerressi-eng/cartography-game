// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MPCDriver.generated.h"

class UMaterialParameterCollection;

/**
 * Pushes gameplay state into a Material Parameter Collection so any
 * material in the world can react. Expects an MPC named "MPC_World"
 * with these scalars:
 *   TimeOfDay         (0..24)
 *   WeatherIntensity  (0..1)
 *   AccuracyGlitch    (0..1)
 *   Fog               (0..1)
 * and these vectors:
 *   WindDirection     (xyz)
 *   BiomeTint         (rgb)
 */
UCLASS()
class CARTOGRAPHYGAME_API UMPCDriver : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC")
	TObjectPtr<UMaterialParameterCollection> Collection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC") float UpdateInterval = 0.2f;

	virtual void Tick(float Dt) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMPCDriver, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate() && Collection != nullptr; }

private:
	float Accum = 0.f;
};
