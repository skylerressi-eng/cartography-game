// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSlotManager.generated.h"

class APawn;

USTRUCT(BlueprintType)
struct FSlotInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FString SlotName;
	UPROPERTY(BlueprintReadOnly) FString DisplayName;
	UPROPERTY(BlueprintReadOnly) FDateTime SavedAt;
	UPROPERTY(BlueprintReadOnly) FString ThumbnailPath;
	UPROPERTY(BlueprintReadOnly) float TimeOfDay = 12.f;
	UPROPERTY(BlueprintReadOnly) bool bIsAuto = false;
};

UCLASS()
class CARTOGRAPHYGAME_API USaveSlotManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Save") TArray<FSlotInfo> ListSlots() const;
	UFUNCTION(BlueprintCallable, Category="Save") bool SaveNamed(const FString& SlotName, const FString& Display, APawn* Player, float TimeOfDay);
	UFUNCTION(BlueprintCallable, Category="Save") bool LoadNamed(const FString& SlotName, APawn* Player, float& OutTimeOfDay);
	UFUNCTION(BlueprintCallable, Category="Save") bool DeleteSlot(const FString& SlotName);
	UFUNCTION(BlueprintCallable, Category="Save") bool QuickSave(APawn* Player, float TimeOfDay);
	UFUNCTION(BlueprintCallable, Category="Save") bool AutoSave(APawn* Player, float TimeOfDay);
	UFUNCTION(BlueprintCallable, Category="Save") FString GetThumbnailPath(const FString& SlotName) const;
};
