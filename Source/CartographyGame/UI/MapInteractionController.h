// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MapInteractionController.generated.h"

/**
 * Tracks pan/zoom state for the field-map widget. Treat normalized UV
 * (0..1) as the *content* coordinate space; the widget renders only the
 * sub-rectangle defined by ViewCenter +/- 0.5/Zoom.
 *
 * The widget feeds the controller raw mouse positions in widget-local
 * pixels. The controller answers back with content-space UVs so drawing
 * lands on the right part of the parchment.
 */
UCLASS(BlueprintType)
class CARTOGRAPHYGAME_API UMapInteractionController : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Map") FVector2D ViewCenter = FVector2D(0.5f, 0.5f);
	UPROPERTY(BlueprintReadWrite, Category="Map") float Zoom = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map") float MinZoom = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map") float MaxZoom = 6.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map") float ZoomStep = 1.25f;

	UFUNCTION(BlueprintCallable, Category="Map") FVector2D ScreenToContentUV(FVector2D ScreenLocal, FVector2D WidgetSize) const;
	UFUNCTION(BlueprintCallable, Category="Map") FVector2D ContentUVToScreen(FVector2D ContentUV, FVector2D WidgetSize) const;
	UFUNCTION(BlueprintCallable, Category="Map") void Pan(FVector2D DeltaScreen, FVector2D WidgetSize);
	UFUNCTION(BlueprintCallable, Category="Map") void ZoomBy(float Multiplier, FVector2D AnchorScreen, FVector2D WidgetSize);
	UFUNCTION(BlueprintCallable, Category="Map") void Reset();
	UFUNCTION(BlueprintCallable, Category="Map") void JumpTo(FVector2D ContentUV, float ZoomLevel);
};
