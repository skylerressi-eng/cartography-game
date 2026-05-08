// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapComparisonComponent.generated.h"

class UTextureRenderTarget2D;

UENUM(BlueprintType)
enum class ECellAccuracy : uint8
{
	Unmapped              UMETA(DisplayName="Unmapped"),
	InaccuratelyMapped    UMETA(DisplayName="Inaccurate"),
	AccuratelyMapped      UMETA(DisplayName="Accurate")
};

USTRUCT(BlueprintType)
struct FCellComparisonResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FIntPoint Cell = FIntPoint::ZeroValue;
	UPROPERTY(BlueprintReadWrite) float Score = 0.f;          // 0..1
	UPROPERTY(BlueprintReadWrite) ECellAccuracy Accuracy = ECellAccuracy::Unmapped;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComparisonComplete, const TArray<FCellComparisonResult>&, Results);

/**
 * Compares the master-map render target to the ground-truth render target by
 * downsampling each into a GridSize x GridSize grid of average colours, then
 * scoring each cell on per-channel similarity.
 *
 * Score interpretation:
 *   Unmapped         — master cell is essentially blank parchment.
 *   Inaccurately     — master cell has marks, but they don't match GT.
 *   Accurately       — master cell matches GT within tolerance.
 *
 * This is intentionally simple but tunable. Run on demand (after an inking
 * session ends), not every frame — it's a CPU readback.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UMapComparisonComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapComparisonComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Comparison", meta=(ClampMin="4", ClampMax="128"))
	int32 GridSize = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Comparison")
	float AccurateThreshold = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Comparison")
	float InaccurateThreshold = 0.35f;

	/** Treat master cell as blank if mean luminance is within this of parchment. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Comparison")
	float BlankLuminanceTolerance = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Comparison")
	FLinearColor ParchmentColor = FLinearColor(0.92f, 0.86f, 0.74f, 1.f);

	UPROPERTY(BlueprintAssignable, Category="Comparison")
	FOnComparisonComplete OnComparisonComplete;

	UPROPERTY(BlueprintReadOnly, Category="Comparison")
	TArray<FCellComparisonResult> LastResults;

	UFUNCTION(BlueprintCallable, Category="Comparison")
	bool RunComparison(UTextureRenderTarget2D* MasterMapRT, UTextureRenderTarget2D* GroundTruthRT);

protected:
	static bool DownsampleRT(UTextureRenderTarget2D* RT, int32 GridSize, TArray<FLinearColor>& OutAverages);
	static float ScoreCell(const FLinearColor& Master, const FLinearColor& GT);
};
