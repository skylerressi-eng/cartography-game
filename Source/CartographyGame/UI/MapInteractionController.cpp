// Copyright (c) 2026 The Last Cartographer.

#include "UI/MapInteractionController.h"

FVector2D UMapInteractionController::ScreenToContentUV(FVector2D ScreenLocal, FVector2D WidgetSize) const
{
	if (WidgetSize.X <= 0.f || WidgetSize.Y <= 0.f) { return FVector2D::ZeroVector; }
	const FVector2D NormScreen = ScreenLocal / WidgetSize;            // 0..1 in widget
	const float Half = 0.5f / FMath::Max(Zoom, 0.0001f);
	return FVector2D(
		FMath::Clamp(ViewCenter.X + (NormScreen.X - 0.5f) * (2.f * Half), 0.f, 1.f),
		FMath::Clamp(ViewCenter.Y + (NormScreen.Y - 0.5f) * (2.f * Half), 0.f, 1.f));
}

FVector2D UMapInteractionController::ContentUVToScreen(FVector2D ContentUV, FVector2D WidgetSize) const
{
	const float Half = 0.5f / FMath::Max(Zoom, 0.0001f);
	const FVector2D NormScreen(0.5f + (ContentUV.X - ViewCenter.X) / (2.f * Half),
	                           0.5f + (ContentUV.Y - ViewCenter.Y) / (2.f * Half));
	return NormScreen * WidgetSize;
}

void UMapInteractionController::Pan(FVector2D DeltaScreen, FVector2D WidgetSize)
{
	if (WidgetSize.X <= 0.f) { return; }
	const float Half = 0.5f / FMath::Max(Zoom, 0.0001f);
	const FVector2D Delta = -DeltaScreen / WidgetSize * (2.f * Half);
	ViewCenter += Delta;
	ViewCenter.X = FMath::Clamp(ViewCenter.X, Half, 1.f - Half);
	ViewCenter.Y = FMath::Clamp(ViewCenter.Y, Half, 1.f - Half);
}

void UMapInteractionController::ZoomBy(float Multiplier, FVector2D AnchorScreen, FVector2D WidgetSize)
{
	const FVector2D AnchorBefore = ScreenToContentUV(AnchorScreen, WidgetSize);
	Zoom = FMath::Clamp(Zoom * Multiplier, MinZoom, MaxZoom);
	const FVector2D AnchorAfter = ScreenToContentUV(AnchorScreen, WidgetSize);
	ViewCenter += (AnchorBefore - AnchorAfter);
	const float Half = 0.5f / FMath::Max(Zoom, 0.0001f);
	ViewCenter.X = FMath::Clamp(ViewCenter.X, Half, 1.f - Half);
	ViewCenter.Y = FMath::Clamp(ViewCenter.Y, Half, 1.f - Half);
}

void UMapInteractionController::Reset() { ViewCenter = FVector2D(0.5f, 0.5f); Zoom = 1.f; }

void UMapInteractionController::JumpTo(FVector2D ContentUV, float ZoomLevel)
{
	Zoom = FMath::Clamp(ZoomLevel, MinZoom, MaxZoom);
	const float Half = 0.5f / FMath::Max(Zoom, 0.0001f);
	ViewCenter.X = FMath::Clamp(ContentUV.X, Half, 1.f - Half);
	ViewCenter.Y = FMath::Clamp(ContentUV.Y, Half, 1.f - Half);
}
