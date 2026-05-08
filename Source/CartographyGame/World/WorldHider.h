// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldHider.generated.h"

class UBuildingHiderComponent;
class UBillboardComponent;

/**
 * Drop one of these in the level. On BeginPlay it scans every actor in
 * the world and hides anything matching the building/prop prefixes. Use
 * this in tandem with the imported Pirate Islands glTF — no need to
 * re-parent the imported scene under a custom actor.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AWorldHider : public AActor
{
	GENERATED_BODY()

public:
	AWorldHider();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hider")
	TObjectPtr<UBillboardComponent> Billboard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hider")
	TObjectPtr<UBuildingHiderComponent> Hider;
};
