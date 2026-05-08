// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DiscoveryLog.generated.h"

USTRUCT(BlueprintType)
struct FDiscoveryEntry
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite) FName Id;
	UPROPERTY(BlueprintReadWrite) FText Name;
	UPROPERTY(BlueprintReadWrite) FName Category;
	UPROPERTY(BlueprintReadWrite) FDateTime Timestamp;
	UPROPERTY(BlueprintReadWrite) FVector WorldLocation = FVector::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscovery, const FDiscoveryEntry&, Entry);

UCLASS()
class CARTOGRAPHYGAME_API UDiscoveryLog : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly,    Category="Discovery") TArray<FDiscoveryEntry> Entries;
	UPROPERTY(BlueprintAssignable, Category="Discovery") FOnDiscovery OnDiscovery;

	UFUNCTION(BlueprintCallable, Category="Discovery")
	void Record(FName Id, FText Name, FName Category, FVector WorldLocation);

	UFUNCTION(BlueprintCallable, Category="Discovery")
	bool HasDiscovered(FName Id) const;

	UFUNCTION(BlueprintCallable, Category="Discovery")
	int32 CountByCategory(FName Category) const;
};
