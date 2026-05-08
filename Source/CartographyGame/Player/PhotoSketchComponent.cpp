// Copyright (c) 2026 The Last Cartographer.

#include "Player/PhotoSketchComponent.h"

#include "CanvasItem.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Map/FieldMapComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Notebook/FieldNotebookComponent.h"
#include "Player/CartographerCharacter.h"

UPhotoSketchComponent::UPhotoSketchComponent() { PrimaryComponentTick.bCanEverTick = false; }

bool UPhotoSketchComponent::CaptureAndStamp()
{
	ACartographerCharacter* Cart = Cast<ACartographerCharacter>(GetOwner());
	if (!Cart || !Cart->FieldMap || !Cart->Notebook || !SketchStampMaterial) { return false; }

	UWorld* World = GetWorld();
	if (!World) { return false; }

	// 1. Build a transient tiny RT to receive the capture.
	UTextureRenderTarget2D* TempRT = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, SketchSize, SketchSize, RTF_RGBA8, FLinearColor::Black, false);
	if (!TempRT) { return false; }

	// 2. One-shot scene capture above the player, looking straight down.
	const FVector Loc = Cart->GetActorLocation() + FVector(0, 0, CaptureHeightAbove);
	const FRotator Rot(-90.f, 0.f, 0.f);
	FActorSpawnParameters Sp; Sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Holder = World->SpawnActor<AActor>(AActor::StaticClass(), Loc, Rot, Sp);
	if (!Holder) { return false; }

	USceneCaptureComponent2D* Cap = NewObject<USceneCaptureComponent2D>(Holder);
	Cap->SetupAttachment(Holder->GetRootComponent());
	Cap->RegisterComponent();
	Cap->ProjectionType = ECameraProjectionMode::Orthographic;
	Cap->OrthoWidth = CaptureOrthoWidth;
	Cap->bCaptureEveryFrame = false;
	Cap->bCaptureOnMovement = false;
	Cap->TextureTarget = TempRT;
	Cap->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	Cap->ShowFlags.SetAtmosphere(false);
	Cap->ShowFlags.SetFog(false);
	Cap->CaptureScene();

	// 3. Stamp TempRT onto FieldMap at the player's map UV through SketchStampMaterial.
	UTextureRenderTarget2D* Field = Cart->FieldMap->FieldMapRT;
	if (!Field) { Holder->Destroy(); return false; }

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(SketchStampMaterial, this);
	MID->SetTextureParameterValue(TEXT("Capture"), TempRT);
	MID->SetVectorParameterValue(TEXT("BrushColor"), InkTint);
	MID->SetScalarParameterValue(TEXT("Opacity"), Opacity);

	const FVector2D UV = Cart->Notebook->WorldToMapUV(Cart->GetActorLocation());
	const float W = (float)Field->SizeX;
	const float H = (float)Field->SizeY;
	const float DiamPx = StampDiameterUV * W;
	const FVector2D TopLeft(UV.X * W - DiamPx * 0.5f, UV.Y * H - DiamPx * 0.5f);

	UCanvas* Canvas; FVector2D CanvasSize; FDrawToRenderTargetContext Ctx;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, Field, Canvas, CanvasSize, Ctx);
	if (Canvas)
	{
		FCanvasTileItem Tile(TopLeft, FVector2D(DiamPx, DiamPx), FLinearColor::White);
		Tile.MaterialRenderProxy = MID->GetRenderProxy();
		Tile.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tile);
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Ctx);

	Holder->Destroy();
	return true;
}
