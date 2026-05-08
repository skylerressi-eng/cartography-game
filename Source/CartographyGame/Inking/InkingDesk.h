// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InkingDesk.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInkingDeskInteract, APawn*, Pawn);

/**
 * Place one of these in the cabin. Hook BP_Cartographer::OnInteractPressed
 * to call TryBeginInking() when the player is overlapping it.
 */
UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AInkingDesk : public AActor
{
	GENERATED_BODY()

public:
	AInkingDesk();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Desk")
	TObjectPtr<UStaticMeshComponent> DeskMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Desk")
	TObjectPtr<UBoxComponent> InteractVolume;

	UPROPERTY(BlueprintAssignable, Category="Desk")
	FOnInkingDeskInteract OnInkingBegan;

	UPROPERTY(BlueprintAssignable, Category="Desk")
	FOnInkingDeskInteract OnInkingEnded;

	UFUNCTION(BlueprintCallable, Category="Desk")
	bool IsPawnInRange(APawn* Pawn) const;

	UFUNCTION(BlueprintCallable, Category="Desk")
	bool TryBeginInking(APawn* Pawn);

	UFUNCTION(BlueprintCallable, Category="Desk")
	void EndInking(APawn* Pawn);

	/** Called by sleep/rest to refill the player's ink. */
	UFUNCTION(BlueprintCallable, Category="Desk")
	void RefillInk(APawn* Pawn) const;
};
