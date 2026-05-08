// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingHider.generated.h"

/**
 * Walks the owning actor's component tree and hides any primitive
 * component whose name (or whose attached child actor's name) matches
 * one of HidePrefixes. Built for the Pirate Islands import: every
 * building/prop is named SM_Bld_* or SM_Prop_*, and the player asked
 * for the islands without buildings.
 *
 * Add this component to BP_PirateIslandWorld. On BeginPlay (or when
 * called manually), it iterates components and toggles their
 * visibility + collision. Exposed property "Reveal" lets you flip
 * the behaviour and unhide a previously hidden subset by id.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UBuildingHiderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuildingHiderComponent();

	/** Hide everything whose name starts with any of these prefixes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hide")
	TArray<FString> HidePrefixes = {
		TEXT("SM_Bld_"),
		TEXT("SM_Prop_")
	};

	/** Names that should remain visible even if they match HidePrefixes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hide")
	TArray<FString> KeepExceptions;

	/** Apply automatically when the game starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hide")
	bool bApplyOnBeginPlay = true;

	/** Reverse the operation: unhide everything matching the prefixes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hide")
	bool bReveal = false;

	UFUNCTION(BlueprintCallable, Category="Hide")
	int32 ApplyToOwner();

	UFUNCTION(BlueprintCallable, Category="Hide")
	int32 ApplyToActor(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="Hide")
	void RevealAll();

protected:
	virtual void BeginPlay() override;

	bool MatchesPrefix(const FString& Name) const;
};
