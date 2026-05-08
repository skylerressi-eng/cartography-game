// Copyright (c) 2026 The Last Cartographer.

#include "Map/SymbolStamp.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

void USymbolStampLibrary::StampSymbol(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* SymbolMaterial,
                                      ECartoSymbol Symbol, FVector2D NormalizedUV, float SizePx,
                                      FLinearColor Color, float Rotation)
{
	if (!World || !RT || !SymbolMaterial) { return; }
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(SymbolMaterial, World);
	if (!MID) { return; }

	MID->SetScalarParameterValue(TEXT("SymbolIndex"), (float)Symbol);
	MID->SetScalarParameterValue(TEXT("Rotation"), Rotation);
	MID->SetVectorParameterValue(TEXT("BrushColor"), Color);

	const float W = (float)RT->SizeX;
	const float H = (float)RT->SizeY;
	const FVector2D Center(NormalizedUV.X * W, NormalizedUV.Y * H);
	const FVector2D Size(SizePx, SizePx);
	const FVector2D TopLeft = Center - Size * 0.5f;

	UCanvas* Canvas; FVector2D CanvasSize; FDrawToRenderTargetContext Ctx;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(World, RT, Canvas, CanvasSize, Ctx);
	if (Canvas)
	{
		FCanvasTileItem Tile(TopLeft, Size, FLinearColor::White);
		Tile.MaterialRenderProxy = MID->GetRenderProxy();
		Tile.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tile);
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(World, Ctx);
}
