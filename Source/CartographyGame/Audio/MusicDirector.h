// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MusicDirector.generated.h"

class UAudioMoodComponent;

/**
 * Picks music mood layer mix from inputs: time of day, weather kind,
 * accuracy at player location, recent discovery flag. Pure logic — relies
 * on a UAudioMoodComponent placed on a HUD actor with layers named
 * "Calm", "Drone", "Wonder", "Storm", "Sleep".
 */
UCLASS()
class CARTOGRAPHYGAME_API UMusicDirector : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Music") TWeakObjectPtr<UAudioMoodComponent> Mixer;
	UPROPERTY(BlueprintReadWrite, Category="Music") float DiscoveryStingerEnergy = 0.f;

	UFUNCTION(BlueprintCallable, Category="Music") void SignalDiscovery() { DiscoveryStingerEnergy = 1.f; }

	virtual void Tick(float DeltaSeconds) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UMusicDirector, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !IsTemplate(); }
};
