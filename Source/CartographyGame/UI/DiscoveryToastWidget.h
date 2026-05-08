// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DiscoveryToastWidget.generated.h"

class UTextBlock;

UCLASS(Abstract, Blueprintable)
class CARTOGRAPHYGAME_API UDiscoveryToastWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> Title_Text;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> Subtitle_Text;

	UFUNCTION(BlueprintCallable, Category="Toast")
	void Show(const FText& Title, const FText& Subtitle, float DurationSeconds = 3.f);

	UFUNCTION(BlueprintImplementableEvent, Category="Toast")
	void OnShow();

	UFUNCTION(BlueprintImplementableEvent, Category="Toast")
	void OnHide();

protected:
	FTimerHandle HideTimer;

	void HideNow();
};
