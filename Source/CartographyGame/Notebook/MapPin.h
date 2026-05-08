// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapPin.generated.h"

class UStaticMeshComponent;
class UBillboardComponent;

UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API AMapPin : public AActor
{
	GENERATED_BODY()

public:
	AMapPin();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pin")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pin", meta=(MultiLine="true"))
	FText Note;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pin")
	FName Category = TEXT("General");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pin")
	FLinearColor MarkerColor = FLinearColor::Red;
};
