// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioMoodComponent.generated.h"

class USoundBase;
class UAudioComponent;

USTRUCT(BlueprintType)
struct FMoodLayer
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<USoundBase> Sound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TargetVolume = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CurrentVolume = 0.f;
	UPROPERTY(VisibleAnywhere) TObjectPtr<UAudioComponent> Component;
};

/**
 * Crossfading ambient/music layer manager. Use one component on the player
 * (or HUD-only actor). Set per-layer target volumes; component crossfades.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UAudioMoodComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAudioMoodComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio") TArray<FMoodLayer> Layers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio") float FadeRate = 0.5f;

	UFUNCTION(BlueprintCallable, Category="Audio") void SetLayerTarget(FName Id, float Volume);
	UFUNCTION(BlueprintCallable, Category="Audio") void StopAll();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick, FActorComponentTickFunction*) override;
};
