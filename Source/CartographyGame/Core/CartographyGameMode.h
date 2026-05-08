// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CartographyGameMode.generated.h"

class UDataTable;

UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API ACartographyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACartographyGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="World") FVector2D WorldOriginXY = FVector2D(-50000, -50000);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="World") FVector2D WorldSizeXY = FVector2D(100000, 100000);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="World") int32 GridSize = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data") TObjectPtr<UDataTable> RegionsTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data") TObjectPtr<UDataTable> BiomesTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data") TObjectPtr<UDataTable> ToolsTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data") TObjectPtr<UDataTable> LandmarksTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data") TObjectPtr<UDataTable> LoreTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data") TObjectPtr<UDataTable> AchievementsTable;

	virtual void StartPlay() override;
};
