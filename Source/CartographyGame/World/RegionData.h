// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RegionData.generated.h"

USTRUCT(BlueprintType)
struct FRegionRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName RegionId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText FlavorText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D OriginXY = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D SizeXY = FVector2D(20000.f, 20000.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName BiomeId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor MapTint = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 LandmarkCount = 0;
};

USTRUCT(BlueprintType)
struct FBiomeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName BiomeId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName AmbientLayer;       // matches AudioMood layer id
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName WildlifeTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor FogTint = FLinearColor(0.6f, 0.65f, 0.7f, 1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeedMultiplier = 1.f;
};

USTRUCT(BlueprintType)
struct FToolRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ToolId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RegionsRequired = 0;
};

USTRUCT(BlueprintType)
struct FLandmarkRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName LandmarkId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName RegionId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName RequiresAccuracy = TEXT("Accurate");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName LoreEntryId;
};

USTRUCT(BlueprintType)
struct FLoreRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName LoreId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true)) FText Body;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Author;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 OrderHint = 0;
};

USTRUCT(BlueprintType)
struct FAchievementRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName AchievementId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHidden = false;
};
