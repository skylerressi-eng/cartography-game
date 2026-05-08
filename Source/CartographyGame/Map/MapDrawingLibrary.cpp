// Copyright (c) 2026 The Last Cartographer.

#include "Map/MapDrawingLibrary.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "HAL/PlatformFileManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "RHI.h"

UMaterialInstanceDynamic* UMapDrawingLibrary::MakeOrReuseMID(UObject* Outer, UMaterialInterface* Source)
{
	if (!Source) { return nullptr; }
	if (UMaterialInstanceDynamic* AsMID = Cast<UMaterialInstanceDynamic>(Source)) { return AsMID; }
	return UMaterialInstanceDynamic::Create(Source, Outer);
}

void UMapDrawingLibrary::StampBrush(UObject* WorldContextObject,
                                    UTextureRenderTarget2D* RenderTarget,
                                    UMaterialInterface* BrushMaterial,
                                    FVector2D NormalizedUV,
                                    float BrushRadiusPx,
                                    FLinearColor BrushColor,
                                    float Opacity,
                                    float Hardness)
{
	if (!RenderTarget || !BrushMaterial || !WorldContextObject) { return; }
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) { return; }

	UMaterialInstanceDynamic* MID = MakeOrReuseMID(WorldContextObject, BrushMaterial);
	if (!MID) { return; }

	MID->SetVectorParameterValue(TEXT("BrushColor"), BrushColor);
	MID->SetScalarParameterValue(TEXT("Opacity"),    Opacity);
	MID->SetScalarParameterValue(TEXT("Hardness"),   Hardness);

	const float W = (float)RenderTarget->SizeX;
	const float H = (float)RenderTarget->SizeY;
	const FVector2D Center(NormalizedUV.X * W, NormalizedUV.Y * H);
	const FVector2D Size(BrushRadiusPx * 2.f, BrushRadiusPx * 2.f);
	const FVector2D TopLeft = Center - FVector2D(BrushRadiusPx, BrushRadiusPx);

	UCanvas* Canvas;
	FVector2D CanvasSize;
	FDrawToRenderTargetContext Ctx;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(WorldContextObject, RenderTarget, Canvas, CanvasSize, Ctx);
	if (Canvas)
	{
		FCanvasTileItem Tile(TopLeft, Size, FLinearColor::White);
		Tile.MaterialRenderProxy = MID->GetRenderProxy();
		Tile.BlendMode = SE_BLEND_TranslucentAlphaOnly;
		Canvas->DrawItem(Tile);
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(WorldContextObject, Ctx);
}

void UMapDrawingLibrary::StampStroke(UObject* WorldContextObject,
                                     UTextureRenderTarget2D* RenderTarget,
                                     UMaterialInterface* BrushMaterial,
                                     FVector2D PrevUV,
                                     FVector2D CurrUV,
                                     float BrushRadiusPx,
                                     FLinearColor BrushColor,
                                     float Opacity,
                                     float Hardness,
                                     float SpacingPx)
{
	if (!RenderTarget || !BrushMaterial || !WorldContextObject) { return; }

	const float W = (float)RenderTarget->SizeX;
	const float H = (float)RenderTarget->SizeY;
	const FVector2D PrevPx(PrevUV.X * W, PrevUV.Y * H);
	const FVector2D CurrPx(CurrUV.X * W, CurrUV.Y * H);
	const float Distance = FVector2D::Distance(PrevPx, CurrPx);
	const float Step = FMath::Max(SpacingPx, 1.f);
	const int32 NumStamps = FMath::Max(1, FMath::CeilToInt(Distance / Step));

	for (int32 i = 1; i <= NumStamps; ++i)
	{
		const float Alpha = (float)i / (float)NumStamps;
		const FVector2D UV = FMath::Lerp(PrevUV, CurrUV, Alpha);
		StampBrush(WorldContextObject, RenderTarget, BrushMaterial, UV, BrushRadiusPx, BrushColor, Opacity, Hardness);
	}
}

void UMapDrawingLibrary::ClearRenderTarget(UObject* WorldContextObject,
                                           UTextureRenderTarget2D* RenderTarget,
                                           FLinearColor ClearColor)
{
	UKismetRenderingLibrary::ClearRenderTarget2D(WorldContextObject, RenderTarget, ClearColor);
}

bool UMapDrawingLibrary::SaveRenderTargetToPNG(UTextureRenderTarget2D* RenderTarget, const FString& AbsoluteFilePath)
{
	if (!RenderTarget) { return false; }

	FTextureRenderTargetResource* RTRes = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTRes) { return false; }

	TArray<FColor> Pixels;
	FReadSurfaceDataFlags Flags(RCM_UNorm, CubeFace_MAX);
	Flags.SetLinearToGamma(true);
	if (!RTRes->ReadPixels(Pixels, Flags)) { return false; }

	IImageWrapperModule& Module = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> Wrapper = Module.CreateImageWrapper(EImageFormat::PNG);
	if (!Wrapper.IsValid()) { return false; }

	if (!Wrapper->SetRaw(Pixels.GetData(), Pixels.GetAllocatedSize(),
	                     RenderTarget->SizeX, RenderTarget->SizeY, ERGBFormat::BGRA, 8))
	{
		return false;
	}

	const TArray64<uint8>& Compressed = Wrapper->GetCompressed(100);
	IPlatformFile::GetPlatformPhysical().CreateDirectoryTree(*FPaths::GetPath(AbsoluteFilePath));
	return FFileHelper::SaveArrayToFile(Compressed, *AbsoluteFilePath);
}

bool UMapDrawingLibrary::LoadPNGToRenderTarget(UObject* WorldContextObject,
                                               UTextureRenderTarget2D* RenderTarget,
                                               const FString& AbsoluteFilePath)
{
	if (!WorldContextObject || !RenderTarget) { return false; }
	UTexture2D* Texture = UKismetRenderingLibrary::ImportFileAsTexture2D(WorldContextObject, AbsoluteFilePath);
	if (!Texture) { return false; }

	UKismetRenderingLibrary::ClearRenderTarget2D(WorldContextObject, RenderTarget, FLinearColor::Transparent);

	UCanvas* Canvas;
	FVector2D CanvasSize;
	FDrawToRenderTargetContext Ctx;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(WorldContextObject, RenderTarget, Canvas, CanvasSize, Ctx);
	if (Canvas)
	{
		FCanvasTileItem Tile(FVector2D::ZeroVector, Texture->GetResource(), CanvasSize, FLinearColor::White);
		Tile.BlendMode = SE_BLEND_Opaque;
		Canvas->DrawItem(Tile);
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(WorldContextObject, Ctx);
	return true;
}
