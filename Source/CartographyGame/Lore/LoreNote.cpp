// Copyright (c) 2026 The Last Cartographer.

#include "Lore/LoreNote.h"

#include "Audio/MusicDirector.h"
#include "Components/StaticMeshComponent.h"
#include "Discovery/DiscoveryLog.h"
#include "Engine/GameInstance.h"
#include "Lore/LoreSubsystem.h"

ALoreNote::ALoreNote()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

FText ALoreNote::GetPrompt_Implementation() const
{
	return Prompt.IsEmpty() ? NSLOCTEXT("Lore", "ReadNote", "Read") : Prompt;
}

void ALoreNote::Interact_Implementation(AActor* /*Instigator*/)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULoreSubsystem* L = GI->GetSubsystem<ULoreSubsystem>())
		{
			L->MarkFound(LoreId);
		}
		if (UDiscoveryLog* D = GI->GetSubsystem<UDiscoveryLog>())
		{
			D->Record(LoreId, FText::FromName(LoreId), TEXT("Lore"), GetActorLocation());
		}
	}
	if (UWorld* World = GetWorld())
	{
		if (UMusicDirector* M = World->GetSubsystem<UMusicDirector>())
		{
			M->SignalDiscovery();
		}
	}
}
