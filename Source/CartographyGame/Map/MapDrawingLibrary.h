// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapDrawingLibrary.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/**
 * Helpers for stamping a brush material onto a render target.
 * Wraps Begin/Draw/End Canvas in a way Blueprints can call easily.
 *
 * Brush material expectations (M_PencilBrush etc.):
 *  - Domain: User Interface
 *  - Blend Mode: Translucent
 *  - Scalar parameters: "Hardness" (0-1), "Opacity" (0-1)
 *  - Vector parameter:  "BrushColor"
 *  - Sampled with the canvas tile UVs (0..1 over the stamp).
 */
UCLASS()
class CARTOGRAPHYGAME_API UMapDrawingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Stamp BrushMaterial onto RT at NormalizedUV with the given pixel radius. */
	UFUNCTION(BlueprintCallable, Category="Cartography|Map", meta=(WorldContext="WorldContextObject"))
	static void StampBrush(UObject* WorldContextObject,
	                       UTextureRenderTarget2D* RenderTarget,
	                       UMaterialInterface* BrushMaterial,
	                       FVector2D NormalizedUV,
	                       float BrushRadiusPx,
	                       FLinearColor BrushColor,
	                       float Opacity = 1.f,
	                       float Hardness = 0.6f);

	/**
	 * Stamp a line of brush dabs from PrevUV to CurrUV. Spacing is in pixels.
	 * Use this every Tick while the user holds the draw button to avoid gaps
	 * when the cursor moves quickly.
	 */
	UFUNCTION(BlueprintCallable, Category="Cartography|Map", meta=(WorldContext="WorldContextObject"))
	static void StampStroke(UObject* WorldContextObject,
	                        UTextureRenderTarget2D* RenderTarget,
	                        UMaterialInterface* BrushMaterial,
	                        FVector2D PrevUV,
	                        FVector2D CurrUV,
	                        float BrushRadiusPx,
	                        FLinearColor BrushColor,
	                        float Opacity = 1.f,
	                        float Hardness = 0.6f,
	                        float SpacingPx = 4.f);

	/** Fill the RT with a single colour (e.g. parchment). */
	UFUNCTION(BlueprintCallable, Category="Cartography|Map", meta=(WorldContext="WorldContextObject"))
	static void ClearRenderTarget(UObject* WorldContextObject,
	                              UTextureRenderTarget2D* RenderTarget,
	                              FLinearColor ClearColor);

	/** Save the RT contents to a PNG on disk (project Saved/MapImages folder). */
	UFUNCTION(BlueprintCallable, Category="Cartography|Map")
	static bool SaveRenderTargetToPNG(UTextureRenderTarget2D* RenderTarget, const FString& AbsoluteFilePath);

	/** Load a PNG from disk and blit it into RT. Sizes must match. */
	UFUNCTION(BlueprintCallable, Category="Cartography|Map", meta=(WorldContext="WorldContextObject"))
	static bool LoadPNGToRenderTarget(UObject* WorldContextObject,
	                                  UTextureRenderTarget2D* RenderTarget,
	                                  const FString& AbsoluteFilePath);

private:
	static UMaterialInstanceDynamic* MakeOrReuseMID(UObject* Outer, UMaterialInterface* Source);
};
