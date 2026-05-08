// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FastTravelMarker.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AFastTravelMarker : public AActor
{
	GENERATED_BODY()

public:
	AFastTravelMarker();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FastTravel") TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FastTravel") TObjectPtr<USphereComponent> ActivationSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FastTravel") FName MarkerId = TEXT("Cabin");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FastTravel") FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FastTravel") bool bRequiresAccurateMapping = true;
	UPROPERTY(BlueprintReadOnly,                  Category="FastTravel") bool bDiscovered = false;
	UPROPERTY(BlueprintReadOnly,                  Category="FastTravel") bool bAvailable = false;

	UFUNCTION(BlueprintCallable, Category="FastTravel") void RefreshAvailability();
	UFUNCTION(BlueprintCallable, Category="FastTravel") void Discover();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);
};
