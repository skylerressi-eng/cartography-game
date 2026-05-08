// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralIsland.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

/**
 * Generates a single-piece island heightmap mesh from layered noise. Use
 * this for instant playable terrain without sculpting in the editor.
 *
 * Place one in the level. On BeginPlay (or when properties change in the
 * editor), it builds a Resolution x Resolution grid covering Size cm,
 * with height shaped by:
 *   1. A radial falloff (so the island sits within Size, surrounded by
 *      water).
 *   2. Three octaves of perlin-style noise.
 *   3. Optional ridged-noise mountain mask near the centre.
 *
 * Below SeaLevel the heightmap clamps to SeaLevel - 50 so a flat water
 * plane reads cleanly.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AProceduralIsland : public AActor
{
	GENERATED_BODY()

public:
	AProceduralIsland();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Island")
	TObjectPtr<UProceduralMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island", meta=(ClampMin=64, ClampMax=512))
	int32 Resolution = 192;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	FVector2D Size = FVector2D(100000.f, 100000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	float MaxAltitude = 9000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	float SeaLevel = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	float NoiseScale = 0.000045f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	float RidgeStrength = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	float CoastFalloffPower = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Island")
	TObjectPtr<UMaterialInterface> LandMaterial;

	UFUNCTION(CallInEditor, Category="Island") void Rebuild();

	UFUNCTION(BlueprintCallable, Category="Island")
	float SampleHeightAt(FVector2D WorldXY) const;

protected:
	virtual void OnConstruction(const FTransform& T) override;
	virtual void BeginPlay() override;

	float HeightFunc(float Nx, float Ny) const;
};
