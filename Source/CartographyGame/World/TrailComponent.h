// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrailComponent.generated.h"

/**
 * Records the player's footsteps as a sparse polyline. Subscribe from the
 * field-map widget to overlay a faint "where you've been" trail.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UTrailComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTrailComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trail") float MinSampleDistance = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trail") int32 MaxSamples = 4096;
	UPROPERTY(BlueprintReadOnly,                 Category="Trail") TArray<FVector> Samples;

	UFUNCTION(BlueprintCallable, Category="Trail") void ResetTrail();
	UFUNCTION(BlueprintCallable, Category="Trail") void SerializeToBytes(TArray<uint8>& Out) const;
	UFUNCTION(BlueprintCallable, Category="Trail") void DeserializeFromBytes(const TArray<uint8>& In);

	virtual void TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*) override;
};
