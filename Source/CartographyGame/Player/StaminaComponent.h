// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, Stamina01);

/**
 * Light hiking-style stamina. Walking is free; jogging or climbing steep
 * terrain drains. Rest at camp/cabin restores. No combat use.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina") float Max = 100.f;
	UPROPERTY(BlueprintReadOnly, Category="Stamina") float Current = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina") float DrainPerSecondJogging = 8.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina") float RegenPerSecondWalking = 4.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stamina") float RegenPerSecondResting = 30.f;

	UPROPERTY(BlueprintAssignable, Category="Stamina") FOnStaminaChanged OnStaminaChanged;

	UFUNCTION(BlueprintCallable, Category="Stamina") void Drain(float Amount);
	UFUNCTION(BlueprintCallable, Category="Stamina") void Restore(float Amount);
	UFUNCTION(BlueprintCallable, Category="Stamina") void TickStamina(float DeltaSeconds, bool bJogging, bool bResting);
	UFUNCTION(BlueprintPure,    Category="Stamina") float Get01() const { return Max > 0.f ? Current / Max : 0.f; }
	UFUNCTION(BlueprintPure,    Category="Stamina") bool IsExhausted() const { return Current <= 0.5f; }
};
