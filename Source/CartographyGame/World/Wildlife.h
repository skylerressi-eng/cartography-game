// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wildlife.generated.h"

class USkeletalMeshComponent;

/**
 * A bird, fish, or deer that wanders within a leash radius. Decorative —
 * it has no AI controller, just a periodic random goal point + a mover.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AWildlife : public AActor
{
	GENERATED_BODY()

public:
	AWildlife();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wildlife") TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditAnywhere,    BlueprintReadWrite, Category="Wildlife") FName SpeciesId = TEXT("Bird");
	UPROPERTY(EditAnywhere,    BlueprintReadWrite, Category="Wildlife") float LeashRadius = 1500.f;
	UPROPERTY(EditAnywhere,    BlueprintReadWrite, Category="Wildlife") float Speed = 200.f;
	UPROPERTY(EditAnywhere,    BlueprintReadWrite, Category="Wildlife") float HomeAltitudeOffset = 0.f;
	UPROPERTY(EditAnywhere,    BlueprintReadWrite, Category="Wildlife") float ReplanInterval = 4.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float Dt) override;

	FVector HomeLocation;
	FVector CurrentGoal;
	float TimeUntilReplan = 0.f;
	void Replan();
};
