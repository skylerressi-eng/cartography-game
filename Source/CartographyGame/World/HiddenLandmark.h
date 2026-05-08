// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundTruth/MapComparisonComponent.h"
#include "HiddenLandmark.generated.h"

class UStaticMeshComponent;

/**
 * Visible only when the cell containing this actor has the required accuracy.
 * The actor binds to UWorldResponseSubsystem::OnCellAccuracyChanged at BeginPlay
 * and toggles its visibility/collision accordingly.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AHiddenLandmark : public AActor
{
	GENERATED_BODY()

public:
	AHiddenLandmark();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Landmark")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Landmark")
	ECellAccuracy MinAccuracyToReveal = ECellAccuracy::AccuratelyMapped;

	UFUNCTION(BlueprintCallable, Category="Landmark")
	void RefreshFromWorldResponse();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleCellChanged(FIntPoint Cell, ECellAccuracy NewAccuracy);

private:
	void ApplyVisibility(ECellAccuracy Accuracy);
	FIntPoint MyCell = FIntPoint::ZeroValue;
};
