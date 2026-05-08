// Copyright (c) 2026 The Last Cartographer.

#include "Map/DrawingTools.h"

#include "Map/MapDrawingLibrary.h"

void UDrawingTools::DrawStraight(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* Brush,
                                 FVector2D StartUV, FVector2D EndUV, float RadiusPx, FLinearColor Color,
                                 float Opacity, float Hardness)
{
	UMapDrawingLibrary::StampStroke(World, RT, Brush, StartUV, EndUV, RadiusPx, Color, Opacity, Hardness, RadiusPx * 0.5f);
}

void UDrawingTools::DrawCircle(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* Brush,
                               FVector2D CenterUV, float RadiusUV, int32 Segments,
                               float StrokeRadiusPx, FLinearColor Color, float Opacity, float Hardness)
{
	const int32 N = FMath::Max(8, Segments);
	FVector2D Prev;
	for (int32 i = 0; i <= N; ++i)
	{
		const float A = (float)i / (float)N * (PI * 2.f);
		const FVector2D P(CenterUV.X + FMath::Cos(A) * RadiusUV, CenterUV.Y + FMath::Sin(A) * RadiusUV);
		if (i > 0)
		{
			UMapDrawingLibrary::StampStroke(World, RT, Brush, Prev, P, StrokeRadiusPx, Color, Opacity, Hardness, StrokeRadiusPx * 0.5f);
		}
		Prev = P;
	}
}

void UDrawingTools::DrawRectangle(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* Brush,
                                  FVector2D MinUV, FVector2D MaxUV, float StrokeRadiusPx,
                                  FLinearColor Color, float Opacity, float Hardness)
{
	const FVector2D TL(MinUV.X, MinUV.Y), TR(MaxUV.X, MinUV.Y), BL(MinUV.X, MaxUV.Y), BR(MaxUV.X, MaxUV.Y);
	DrawStraight(World, RT, Brush, TL, TR, StrokeRadiusPx, Color, Opacity, Hardness);
	DrawStraight(World, RT, Brush, TR, BR, StrokeRadiusPx, Color, Opacity, Hardness);
	DrawStraight(World, RT, Brush, BR, BL, StrokeRadiusPx, Color, Opacity, Hardness);
	DrawStraight(World, RT, Brush, BL, TL, StrokeRadiusPx, Color, Opacity, Hardness);
}
