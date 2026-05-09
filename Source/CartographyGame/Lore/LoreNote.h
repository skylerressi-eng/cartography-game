// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/InteractionComponent.h"
#include "LoreNote.generated.h"

class UStaticMeshComponent;

/**
 * Worldspace lore object. Walk up to it, press E, and the LoreSubsystem
 * marks LoreId as found + DiscoveryLog records it. Place wherever a
 * note/parchment/plaque belongs in the world. The LoreId field maps to
 * a row in DT_Lore.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API ALoreNote : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ALoreNote();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lore")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lore")
	FName LoreId = TEXT("Beacon01");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lore")
	FText Prompt;

	// IInteractable
	virtual FText GetPrompt_Implementation() const override;
	virtual void  Interact_Implementation(AActor* Instigator) override;
};
