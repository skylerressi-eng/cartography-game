// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FieldNotebookComponent.generated.h"

class AMapPin;

USTRUCT(BlueprintType)
struct FNotebookEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine="true")) FText Body;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Category = TEXT("General");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor MarkerColor = FLinearColor::Red;
	/** Set after the entry has been written to a pin actor. */
	UPROPERTY(BlueprintReadWrite) TWeakObjectPtr<AMapPin> SpawnedPin;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotebookChanged, int32, ChangedIndex);

UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UFieldNotebookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFieldNotebookComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Notebook")
	TSubclassOf<AMapPin> PinClass;

	UPROPERTY(BlueprintReadOnly, Category="Notebook")
	TArray<FNotebookEntry> Entries;

	UPROPERTY(BlueprintAssignable, Category="Notebook")
	FOnNotebookChanged OnNotebookChanged;

	/** World bounds the field map represents (XY). Used to project pins to map UV. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Notebook|World")
	FVector WorldOriginXY = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Notebook|World")
	FVector2D WorldSizeXY = FVector2D(100000.f, 100000.f);

	UFUNCTION(BlueprintCallable, Category="Notebook")
	int32 AddEntry(FText Title, FText Body, FVector WorldLocation, FName Category);

	UFUNCTION(BlueprintCallable, Category="Notebook")
	int32 PinAtPlayerLocation(FText Title, FText Body, FName Category);

	UFUNCTION(BlueprintCallable, Category="Notebook")
	void RemoveEntry(int32 Index);

	UFUNCTION(BlueprintPure, Category="Notebook")
	FVector2D WorldToMapUV(FVector WorldLocation) const;
};
