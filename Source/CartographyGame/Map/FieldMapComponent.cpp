// Copyright (c) 2026 The Last Cartographer.

#include "Map/FieldMapComponent.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Map/MapDrawingLibrary.h"

UFieldMapComponent::UFieldMapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PencilSettings.RadiusPx = 6.f;
	PencilSettings.Color    = FLinearColor(0.10f, 0.08f, 0.07f, 1.f);
	PencilSettings.Opacity  = 0.55f;
	PencilSettings.Hardness = 0.4f;
	PencilSettings.SpacingPx = 3.f;

	EraserSettings.RadiusPx = 24.f;
	EraserSettings.Color    = FLinearColor(0.92f, 0.86f, 0.74f, 1.f);
	EraserSettings.Opacity  = 1.f;
	EraserSettings.Hardness = 0.5f;
	EraserSettings.SpacingPx = 6.f;

	InkSettings.RadiusPx    = 4.f;
	InkSettings.Color       = FLinearColor(0.04f, 0.03f, 0.02f, 1.f);
	InkSettings.Opacity     = 1.f;
	InkSettings.Hardness    = 0.95f;
	InkSettings.SpacingPx   = 2.f;
}

UTextureRenderTarget2D* UFieldMapComponent::EnsureRenderTarget(UTextureRenderTarget2D*& Slot, const FName& /*Tag*/)
{
	if (Slot) { return Slot; }

	Slot = UKismetRenderingLibrary::CreateRenderTarget2D(this, RenderTargetSize, RenderTargetSize, RTF_RGBA8, ParchmentColor, /*bAutoGenerateMipMaps*/ false);
	if (Slot)
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, Slot, ParchmentColor);
	}
	return Slot;
}

void UFieldMapComponent::BeginPlay()
{
	Super::BeginPlay();
	EnsureRenderTarget(FieldMapRT, TEXT("FieldMapRT"));
	EnsureRenderTarget(MasterMapRT, TEXT("MasterMapRT"));
	CurrentInk = MaxInk;
}

UTextureRenderTarget2D* UFieldMapComponent::GetRenderTarget(ECartoMapTarget Target) const
{
	return Target == ECartoMapTarget::FieldMap ? FieldMapRT.Get() : MasterMapRT.Get();
}

UMaterialInterface* UFieldMapComponent::GetBrushMaterial(ECartoBrushMode Mode) const
{
	switch (Mode)
	{
	case ECartoBrushMode::Pencil: return PencilBrushMaterial;
	case ECartoBrushMode::Eraser: return EraserBrushMaterial;
	case ECartoBrushMode::Ink:    return InkBrushMaterial;
	}
	return nullptr;
}

const FCartoBrushSettings& UFieldMapComponent::GetBrushSettings(ECartoBrushMode Mode) const
{
	switch (Mode)
	{
	case ECartoBrushMode::Pencil: return PencilSettings;
	case ECartoBrushMode::Eraser: return EraserSettings;
	case ECartoBrushMode::Ink:    return InkSettings;
	}
	return PencilSettings;
}

void UFieldMapComponent::DrawAt(FVector2D NormalizedUV, ECartoMapTarget Target, ECartoBrushMode Mode)
{
	UTextureRenderTarget2D* RT = GetRenderTarget(Target);
	UMaterialInterface* Brush = GetBrushMaterial(Mode);
	if (!RT || !Brush) { return; }

	if (Target == ECartoMapTarget::MasterMap && Mode == ECartoBrushMode::Ink)
	{
		if (CurrentInk <= 0.f) { return; }
		CurrentInk = FMath::Max(0.f, CurrentInk - InkPerStrokeStamp);
	}

	const FCartoBrushSettings& S = GetBrushSettings(Mode);
	UMapDrawingLibrary::StampBrush(this, RT, Brush, NormalizedUV, S.RadiusPx, S.Color, S.Opacity, S.Hardness);
}

void UFieldMapComponent::DrawStroke(FVector2D PrevUV, FVector2D CurrUV, ECartoMapTarget Target, ECartoBrushMode Mode)
{
	UTextureRenderTarget2D* RT = GetRenderTarget(Target);
	UMaterialInterface* Brush = GetBrushMaterial(Mode);
	if (!RT || !Brush) { return; }

	if (Target == ECartoMapTarget::MasterMap && Mode == ECartoBrushMode::Ink)
	{
		if (CurrentInk <= 0.f) { return; }
	}

	const FCartoBrushSettings& S = GetBrushSettings(Mode);
	UMapDrawingLibrary::StampStroke(this, RT, Brush, PrevUV, CurrUV, S.RadiusPx, S.Color, S.Opacity, S.Hardness, S.SpacingPx);

	if (Target == ECartoMapTarget::MasterMap && Mode == ECartoBrushMode::Ink)
	{
		const float Length = FVector2D::Distance(PrevUV, CurrUV) * (float)RT->SizeX;
		const int32 Stamps = FMath::Max(1, FMath::CeilToInt(Length / FMath::Max(S.SpacingPx, 1.f)));
		CurrentInk = FMath::Max(0.f, CurrentInk - InkPerStrokeStamp * Stamps);
	}
}

void UFieldMapComponent::ClearMap(ECartoMapTarget Target)
{
	UTextureRenderTarget2D* RT = GetRenderTarget(Target);
	if (!RT) { return; }
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RT, ParchmentColor);
}

void UFieldMapComponent::RestoreInk(float Amount)
{
	CurrentInk = FMath::Clamp(CurrentInk + Amount, 0.f, MaxInk);
}

void UFieldMapComponent::RefillInkToMax()
{
	CurrentInk = MaxInk;
}
