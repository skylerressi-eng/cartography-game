// Copyright (c) 2026 The Last Cartographer.

#include "GroundTruth/MapComparisonComponent.h"

#include "Engine/TextureRenderTarget2D.h"

UMapComparisonComponent::UMapComparisonComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UMapComparisonComponent::DownsampleRT(UTextureRenderTarget2D* RT, int32 GridSize, TArray<FLinearColor>& OutAverages)
{
	OutAverages.Reset();
	if (!RT || GridSize <= 0) { return false; }

	FTextureRenderTargetResource* Res = RT->GameThread_GetRenderTargetResource();
	if (!Res) { return false; }

	const int32 W = RT->SizeX;
	const int32 H = RT->SizeY;

	TArray<FColor> Pixels;
	FReadSurfaceDataFlags Flags(RCM_UNorm, CubeFace_MAX);
	Flags.SetLinearToGamma(false);
	if (!Res->ReadPixels(Pixels, Flags)) { return false; }
	if (Pixels.Num() != W * H) { return false; }

	OutAverages.SetNumZeroed(GridSize * GridSize);
	const float CellW = (float)W / (float)GridSize;
	const float CellH = (float)H / (float)GridSize;

	for (int32 cy = 0; cy < GridSize; ++cy)
	{
		const int32 Y0 = FMath::FloorToInt(cy * CellH);
		const int32 Y1 = FMath::Min(H, FMath::FloorToInt((cy + 1) * CellH));
		for (int32 cx = 0; cx < GridSize; ++cx)
		{
			const int32 X0 = FMath::FloorToInt(cx * CellW);
			const int32 X1 = FMath::Min(W, FMath::FloorToInt((cx + 1) * CellW));

			double R = 0, G = 0, B = 0, A = 0;
			int32 N = 0;
			for (int32 y = Y0; y < Y1; ++y)
			{
				for (int32 x = X0; x < X1; ++x)
				{
					const FColor& C = Pixels[y * W + x];
					R += C.R; G += C.G; B += C.B; A += C.A;
					++N;
				}
			}
			if (N == 0) { N = 1; }
			OutAverages[cy * GridSize + cx] = FLinearColor(
				(float)(R / 255.0 / N),
				(float)(G / 255.0 / N),
				(float)(B / 255.0 / N),
				(float)(A / 255.0 / N));
		}
	}
	return true;
}

float UMapComparisonComponent::ScoreCell(const FLinearColor& Master, const FLinearColor& GT)
{
	// Per-channel L1 similarity. The ground-truth PP material encodes
	// categories on RGBA, so channel-wise distance is meaningful.
	const float dR = FMath::Abs(Master.R - GT.R);
	const float dG = FMath::Abs(Master.G - GT.G);
	const float dB = FMath::Abs(Master.B - GT.B);
	const float dA = FMath::Abs(Master.A - GT.A);
	const float Mean = (dR + dG + dB + dA) * 0.25f;
	return FMath::Clamp(1.f - Mean, 0.f, 1.f);
}

bool UMapComparisonComponent::RunComparison(UTextureRenderTarget2D* MasterMapRT, UTextureRenderTarget2D* GroundTruthRT)
{
	TArray<FLinearColor> Master, GT;
	if (!DownsampleRT(MasterMapRT, GridSize, Master)) { return false; }
	if (!DownsampleRT(GroundTruthRT, GridSize, GT))   { return false; }
	if (Master.Num() != GT.Num()) { return false; }

	LastResults.Reset();
	LastResults.Reserve(Master.Num());

	const float ParchmentLuma = 0.2126f * ParchmentColor.R + 0.7152f * ParchmentColor.G + 0.0722f * ParchmentColor.B;

	for (int32 i = 0; i < Master.Num(); ++i)
	{
		const int32 cx = i % GridSize;
		const int32 cy = i / GridSize;

		FCellComparisonResult Result;
		Result.Cell = FIntPoint(cx, cy);

		const float MasterLuma = 0.2126f * Master[i].R + 0.7152f * Master[i].G + 0.0722f * Master[i].B;
		const bool bBlank = FMath::Abs(MasterLuma - ParchmentLuma) < BlankLuminanceTolerance;

		if (bBlank)
		{
			Result.Score = 0.f;
			Result.Accuracy = ECellAccuracy::Unmapped;
		}
		else
		{
			Result.Score = ScoreCell(Master[i], GT[i]);
			if (Result.Score >= AccurateThreshold)
			{
				Result.Accuracy = ECellAccuracy::AccuratelyMapped;
			}
			else if (Result.Score >= InaccurateThreshold)
			{
				Result.Accuracy = ECellAccuracy::InaccuratelyMapped;
			}
			else
			{
				Result.Accuracy = ECellAccuracy::InaccuratelyMapped;
			}
		}
		LastResults.Add(Result);
	}

	OnComparisonComplete.Broadcast(LastResults);
	return true;
}
