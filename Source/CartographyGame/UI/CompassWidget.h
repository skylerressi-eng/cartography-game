// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompassWidget.generated.h"

class UImage;

/**
 * Tiny compass that rotates a north arrow (Image named "Arrow_Image"
 * in the BP child) opposite the camera yaw. Designer extends BP_Compass
 * from this class and lays out the visuals.
 */
UCLASS(Abstract, Blueprintable)
class CARTOGRAPHYGAME_API UCompassWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly) TObjectPtr<UImage> Arrow_Image;

	UFUNCTION(BlueprintImplementableEvent, Category="Compass")
	void OnHeadingChanged(float YawDegrees);

	virtual void NativeTick(const FGeometry&, float InDeltaTime) override;
};
