// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhotoSketchComponent.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UMaterialInterface;

/**
 * "Sketch what you see." Spawns a one-shot top-down scene-capture above
 * the player, renders the local terrain to a tiny RT (e.g. 256x256),
 * then stamps that RT (multiplied by a soft brush) into the field map
 * at the player's projected map UV. The output looks like a quick,
 * imperfect ink wash — perfect for the "rough memory aid" framing.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UPhotoSketchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPhotoSketchComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sketch") int32 SketchSize = 256;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sketch") float CaptureOrthoWidth = 6000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sketch") float CaptureHeightAbove = 5000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sketch") float StampDiameterUV = 0.06f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sketch") FLinearColor InkTint = FLinearColor(0.10f, 0.08f, 0.07f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sketch") float Opacity = 0.3f;

	/** Material that takes a Texture parameter "Capture" and modulates by InkTint. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sketch") TObjectPtr<UMaterialInterface> SketchStampMaterial;

	UFUNCTION(BlueprintCallable, Category="Sketch")
	bool CaptureAndStamp();
};
