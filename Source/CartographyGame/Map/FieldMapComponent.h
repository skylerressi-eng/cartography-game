// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FieldMapComponent.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;

UENUM(BlueprintType)
enum class ECartoMapTarget : uint8
{
	FieldMap     UMETA(DisplayName="Field Map (parchment)"),
	MasterMap    UMETA(DisplayName="Master Map (inked)")
};

UENUM(BlueprintType)
enum class ECartoBrushMode : uint8
{
	Pencil       UMETA(DisplayName="Pencil"),
	Eraser       UMETA(DisplayName="Eraser"),
	Ink          UMETA(DisplayName="Ink")
};

USTRUCT(BlueprintType)
struct FCartoBrushSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RadiusPx = 6.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor Color = FLinearColor(0.10f, 0.08f, 0.07f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Opacity = 0.55f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Hardness = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SpacingPx = 3.f;
};

UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UFieldMapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFieldMapComponent();

	// --- Render targets -----------------------------------------------------
	/** Optional asset reference. If null, a transient RT is created at BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|RenderTargets")
	TObjectPtr<UTextureRenderTarget2D> FieldMapRT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|RenderTargets")
	TObjectPtr<UTextureRenderTarget2D> MasterMapRT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|RenderTargets")
	int32 RenderTargetSize = 2048;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|RenderTargets")
	FLinearColor ParchmentColor = FLinearColor(0.92f, 0.86f, 0.74f, 1.f);

	// --- Brush materials (assign in editor) ---------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Brushes")
	TObjectPtr<UMaterialInterface> PencilBrushMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Brushes")
	TObjectPtr<UMaterialInterface> EraserBrushMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Brushes")
	TObjectPtr<UMaterialInterface> InkBrushMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Brushes")
	FCartoBrushSettings PencilSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Brushes")
	FCartoBrushSettings EraserSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Brushes")
	FCartoBrushSettings InkSettings;

	// --- Ink resource (System 4) -------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Ink", meta=(ClampMin="0"))
	float MaxInk = 100.f;

	UPROPERTY(BlueprintReadOnly, Category="Cartography|Ink")
	float CurrentInk = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cartography|Ink", meta=(ClampMin="0"))
	float InkPerStrokeStamp = 0.05f;

	// --- API ---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category="Cartography")
	void DrawAt(FVector2D NormalizedUV, ECartoMapTarget Target, ECartoBrushMode Mode);

	UFUNCTION(BlueprintCallable, Category="Cartography")
	void DrawStroke(FVector2D PrevUV, FVector2D CurrUV, ECartoMapTarget Target, ECartoBrushMode Mode);

	UFUNCTION(BlueprintCallable, Category="Cartography")
	void ClearMap(ECartoMapTarget Target);

	UFUNCTION(BlueprintCallable, Category="Cartography")
	UTextureRenderTarget2D* GetRenderTarget(ECartoMapTarget Target) const;

	UFUNCTION(BlueprintCallable, Category="Cartography|Ink")
	void RestoreInk(float Amount);

	UFUNCTION(BlueprintCallable, Category="Cartography|Ink")
	void RefillInkToMax();

protected:
	virtual void BeginPlay() override;

	UTextureRenderTarget2D* EnsureRenderTarget(UTextureRenderTarget2D*& Slot, const FName& Tag);
	UMaterialInterface* GetBrushMaterial(ECartoBrushMode Mode) const;
	const FCartoBrushSettings& GetBrushSettings(ECartoBrushMode Mode) const;
};
