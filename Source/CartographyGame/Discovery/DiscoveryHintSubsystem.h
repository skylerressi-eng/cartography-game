// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DiscoveryHintSubsystem.generated.h"

class USoundBase;
class AHiddenLandmark;

/**
 * Plays a quiet "almost there" cue when the player wanders close to an
 * undiscovered HiddenLandmark whose cell is currently inaccurate. Goal:
 * gentle hint that mapping this area would reveal something, without
 * spelling out where.
 */
UCLASS()
class CARTOGRAPHYGAME_API UDiscoveryHintSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hint") TObjectPtr<USoundBase> WhisperSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hint") float ProbeRadius = 4500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hint") float CooldownSeconds = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hint") float Volume = 0.4f;

	virtual void Tick(float Dt) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDiscoveryHintSubsystem, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }

private:
	float Cooldown = 0.f;
};
