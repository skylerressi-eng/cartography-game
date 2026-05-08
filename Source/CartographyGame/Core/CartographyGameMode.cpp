// Copyright (c) 2026 The Last Cartographer.

#include "Core/CartographyGameMode.h"

#include "Achievements/AchievementSubsystem.h"
#include "Engine/GameInstance.h"
#include "Lore/LoreSubsystem.h"
#include "World/BiomeManager.h"
#include "World/WorldResponseSubsystem.h"

ACartographyGameMode::ACartographyGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACartographyGameMode::StartPlay()
{
	Super::StartPlay();

	if (UWorld* World = GetWorld())
	{
		if (UWorldResponseSubsystem* Wr = World->GetSubsystem<UWorldResponseSubsystem>())
		{
			Wr->SetGridConfig(WorldOriginXY, WorldSizeXY, GridSize);
		}
		if (UBiomeManager* Bm = World->GetSubsystem<UBiomeManager>())
		{
			Bm->RegionsTable = RegionsTable;
			Bm->BiomesTable = BiomesTable;
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULoreSubsystem* L = GI->GetSubsystem<ULoreSubsystem>()) { L->LoreTable = LoreTable; }
		if (UAchievementSubsystem* A = GI->GetSubsystem<UAchievementSubsystem>()) { A->AchievementsTable = AchievementsTable; }
	}
}
