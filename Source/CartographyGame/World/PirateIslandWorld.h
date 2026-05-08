// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PirateIslandWorld.generated.h"

class UBuildingHiderComponent;
class UStaticMesh;

/**
 * Wraps the imported Pirate Islands glTF as a single placeable actor.
 * The Python importer (Tools/import_pirate_islands.py) creates a
 * BP_PirateIslandWorld whose root has thousands of UStaticMeshComponents
 * spawned from the glTF, plus a BuildingHiderComponent that hides any
 * SM_Bld_* / SM_Prop_* on BeginPlay.
 *
 * Setting WorldExtentXY here (in cm) lets the GameMode and the
 * NotebookComponent compute the same world-to-map UV projection so
 * pins line up with the actual island.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API APirateIslandWorld : public AActor
{
	GENERATED_BODY()

public:
	APirateIslandWorld();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Island")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Island")
	TObjectPtr<UBuildingHiderComponent> Hider;

	/** XY extents in cm. Used by GameMode to drive WorldOriginXY/SizeXY. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	FVector2D WorldExtentXY = FVector2D(40000.f, 40000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	float SeaLevel = 0.f;

	UFUNCTION(BlueprintCallable, Category="Island") FBox ComputeWorldBounds() const;
	UFUNCTION(BlueprintCallable, Category="Island") FVector2D GetWorldOriginXY() const;
};
