// Copyright (c) 2026 The Last Cartographer.

#include "GroundTruth/GroundTruthCapture.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInterface.h"

AGroundTruthCapture::AGroundTruthCapture()
{
	PrimaryActorTick.bCanEverTick = false;

	Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	RootComponent = Capture;
	Capture->ProjectionType    = ECameraProjectionMode::Orthographic;
	Capture->OrthoWidth        = OrthoWidth;
	Capture->bCaptureEveryFrame = false;
	Capture->bCaptureOnMovement = false;
	Capture->CaptureSource     = ESceneCaptureSource::SCS_FinalColorLDR;
	Capture->ShowFlags.SetAtmosphere(false);
	Capture->ShowFlags.SetFog(false);
	Capture->ShowFlags.SetVolumetricFog(false);
}

void AGroundTruthCapture::EnsureRT()
{
	if (!GroundTruthRT)
	{
		GroundTruthRT = UKismetRenderingLibrary::CreateRenderTarget2D(this, RenderTargetSize, RenderTargetSize, RTF_RGBA8, FLinearColor::Black, false);
	}
	if (Capture)
	{
		Capture->TextureTarget = GroundTruthRT;
		Capture->OrthoWidth = OrthoWidth;
		if (ClassificationPPMaterial)
		{
			Capture->PostProcessSettings.AddBlendable(ClassificationPPMaterial, 1.f);
		}
	}
}

void AGroundTruthCapture::BeginPlay()
{
	Super::BeginPlay();
	EnsureRT();
}

void AGroundTruthCapture::CaptureNow()
{
	EnsureRT();
	if (Capture)
	{
		Capture->CaptureScene();
	}
}
