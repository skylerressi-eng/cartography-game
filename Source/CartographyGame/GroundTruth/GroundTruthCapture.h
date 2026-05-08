// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundTruthCapture.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UMaterialInterface;

/**
 * Top-down orthographic capture of the world that bakes "ground truth" into a
 * render target. The scene capture should be configured to use a
 * post-process material (M_PP_GroundTruth) that emits category colours:
 *   R = land, G = water, B = peaks, A = special features.
 *
 * Place one of these high above the island, point straight down, set ortho
 * width to cover the playable area. Call CaptureNow() once at startup
 * (and after major terrain changes) — the comparison system reads the RT.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AGroundTruthCapture : public AActor
{
	GENERATED_BODY()

public:
	AGroundTruthCapture();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GroundTruth")
	TObjectPtr<USceneCaptureComponent2D> Capture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundTruth")
	TObjectPtr<UTextureRenderTarget2D> GroundTruthRT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundTruth")
	int32 RenderTargetSize = 2048;

	/**
	 * Post-process material that classifies the scene into category colours.
	 * Required for the comparison system to work meaningfully.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundTruth")
	TObjectPtr<UMaterialInterface> ClassificationPPMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GroundTruth")
	float OrthoWidth = 100000.f;

	UFUNCTION(BlueprintCallable, Category="GroundTruth")
	void CaptureNow();

protected:
	virtual void BeginPlay() override;
	void EnsureRT();
};
