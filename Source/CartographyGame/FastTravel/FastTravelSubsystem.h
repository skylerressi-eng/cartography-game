// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FastTravelSubsystem.generated.h"

class AFastTravelMarker;
class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFastTravelDiscovered, AFastTravelMarker*, Marker);

UCLASS()
class CARTOGRAPHYGAME_API UFastTravelSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="FastTravel") FOnFastTravelDiscovered OnDiscovered;
	UPROPERTY(BlueprintReadOnly,    Category="FastTravel") TArray<TWeakObjectPtr<AFastTravelMarker>> Markers;

	UFUNCTION(BlueprintCallable, Category="FastTravel") void RegisterMarker(AFastTravelMarker* Marker);
	UFUNCTION(BlueprintCallable, Category="FastTravel") TArray<AFastTravelMarker*> GetAvailable() const;
	UFUNCTION(BlueprintCallable, Category="FastTravel") bool TravelTo(APawn* Pawn, FName MarkerId);
};
