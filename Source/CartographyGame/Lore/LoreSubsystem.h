// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "World/RegionData.h"
#include "LoreSubsystem.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoreFound, FName, LoreId);

UCLASS()
class CARTOGRAPHYGAME_API ULoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lore") TObjectPtr<UDataTable> LoreTable;
	UPROPERTY(BlueprintReadOnly,                 Category="Lore") TSet<FName> Found;

	UPROPERTY(BlueprintAssignable, Category="Lore") FOnLoreFound OnLoreFound;

	UFUNCTION(BlueprintCallable, Category="Lore") void MarkFound(FName LoreId);
	UFUNCTION(BlueprintCallable, Category="Lore") FLoreRow GetEntry(FName LoreId) const;
	UFUNCTION(BlueprintCallable, Category="Lore") TArray<FLoreRow> GetAllFoundOrdered() const;
	UFUNCTION(BlueprintCallable, Category="Lore") void LoadFound(const TArray<FName>& Ids);
};
