// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "World/RegionData.h"
#include "AchievementSubsystem.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAchievementUnlocked, const FAchievementRow&, Row);

UCLASS()
class CARTOGRAPHYGAME_API UAchievementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Achievements") TObjectPtr<UDataTable> AchievementsTable;
	UPROPERTY(BlueprintReadOnly,                 Category="Achievements") TSet<FName> Unlocked;
	UPROPERTY(BlueprintAssignable,               Category="Achievements") FOnAchievementUnlocked OnUnlocked;

	UFUNCTION(BlueprintCallable, Category="Achievements") bool Unlock(FName AchievementId);
	UFUNCTION(BlueprintPure,    Category="Achievements") bool IsUnlocked(FName Id) const { return Unlocked.Contains(Id); }
	UFUNCTION(BlueprintCallable, Category="Achievements") FAchievementRow GetRow(FName Id) const;
	UFUNCTION(BlueprintCallable, Category="Achievements") TArray<FName> GetAllUnlocked() const { return Unlocked.Array(); }
	UFUNCTION(BlueprintCallable, Category="Achievements") void Load(const TArray<FName>& Ids);
};
