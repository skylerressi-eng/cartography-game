// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DrawingTools.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;

/**
 * Higher-level drawing primitives that compose StampStroke calls:
 *   - straight-edge ruler segments
 *   - perfect circles (compass tool)
 *   - rectangles (for inking grid references)
 */
UCLASS()
class CARTOGRAPHYGAME_API UDrawingTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Cartography|Tools", meta=(WorldContext="World"))
	static void DrawStraight(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* Brush,
	                         FVector2D StartUV, FVector2D EndUV, float RadiusPx, FLinearColor Color,
	                         float Opacity = 1.f, float Hardness = 0.6f);

	UFUNCTION(BlueprintCallable, Category="Cartography|Tools", meta=(WorldContext="World"))
	static void DrawCircle(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* Brush,
	                       FVector2D CenterUV, float RadiusUV, int32 Segments,
	                       float StrokeRadiusPx, FLinearColor Color,
	                       float Opacity = 1.f, float Hardness = 0.6f);

	UFUNCTION(BlueprintCallable, Category="Cartography|Tools", meta=(WorldContext="World"))
	static void DrawRectangle(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* Brush,
	                          FVector2D MinUV, FVector2D MaxUV,
	                          float StrokeRadiusPx, FLinearColor Color,
	                          float Opacity = 1.f, float Hardness = 0.6f);
};
