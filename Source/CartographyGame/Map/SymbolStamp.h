// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SymbolStamp.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;

UENUM(BlueprintType)
enum class ECartoSymbol : uint8
{
	Mountain    UMETA(DisplayName="Mountain"),
	Forest      UMETA(DisplayName="Forest"),
	River       UMETA(DisplayName="River"),
	Ruin        UMETA(DisplayName="Ruin"),
	Camp        UMETA(DisplayName="Camp"),
	NorthArrow  UMETA(DisplayName="North"),
	X           UMETA(DisplayName="X marks the spot"),
	Custom      UMETA(DisplayName="Custom")
};

/**
 * Stamps map symbols onto a render target using a single shared material
 * (M_SymbolStamp) parameterised by an integer SymbolIndex matching the
 * enum ordering. Designers author a stamp atlas; index picks the cell.
 */
UCLASS()
class CARTOGRAPHYGAME_API USymbolStampLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Cartography|Symbols", meta=(WorldContext="World"))
	static void StampSymbol(UObject* World, UTextureRenderTarget2D* RT, UMaterialInterface* SymbolMaterial,
	                        ECartoSymbol Symbol, FVector2D NormalizedUV, float SizePx,
	                        FLinearColor Color, float Rotation = 0.f);
};
