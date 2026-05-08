// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapBookmark.generated.h"

USTRUCT(BlueprintType)
struct FMapBookmark
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D MapUV = FVector2D::ZeroVector;   // 0..1
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Zoom = 1.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBookmarksChanged);

/**
 * Stores user-defined viewport positions on the field map (e.g. "Cabin",
 * "South Cape"). Field-map widget reads these to populate a quick-jump menu.
 */
UCLASS(ClassGroup=(Cartography), meta=(BlueprintSpawnableComponent))
class CARTOGRAPHYGAME_API UMapBookmarkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Bookmarks") TArray<FMapBookmark> Bookmarks;
	UPROPERTY(BlueprintAssignable, Category="Bookmarks") FOnBookmarksChanged OnBookmarksChanged;

	UFUNCTION(BlueprintCallable, Category="Bookmarks") int32 Add(FName Id, FText Name, FVector2D MapUV, float Zoom);
	UFUNCTION(BlueprintCallable, Category="Bookmarks") void Remove(FName Id);
	UFUNCTION(BlueprintCallable, Category="Bookmarks") FMapBookmark Get(FName Id) const;
};
