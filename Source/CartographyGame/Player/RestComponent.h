// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RestComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRested, float, HoursPassed);

/**
 * Sleep / rest. Player approaches a bed/campfire actor that calls
 * BeginRest, the screen fades, time advances, ink is replenished.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API URestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URestComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rest") float SleepHours = 8.f;
	UPROPERTY(BlueprintAssignable, Category="Rest") FOnRested OnRested;

	UFUNCTION(BlueprintCallable, Category="Rest") void BeginRestUntilMorning();
	UFUNCTION(BlueprintCallable, Category="Rest") void BeginQuickRest(float Hours);
};
