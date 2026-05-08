// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "CartoUserSettings.generated.h"

UCLASS()
class CARTOGRAPHYGAME_API UCartoUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UCartoUserSettings();

	// --- Audio ----
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Audio") float MasterVolume = 1.f;
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Audio") float MusicVolume = 0.8f;
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Audio") float AmbientVolume = 1.f;
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Audio") float SfxVolume = 1.f;

	// --- Gameplay ----
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Gameplay") float MouseSensitivity = 1.f;
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Gameplay") bool bInvertY = false;
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Gameplay") bool bShowAccuracyHints = false;
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|Gameplay") float HoursPerSecondOverride = 0.f;

	// --- UI ----
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|UI") float UIScale = 1.f;
	UPROPERTY(Config, BlueprintReadWrite, Category="Settings|UI") bool bShowCompass = true;

	UFUNCTION(BlueprintCallable, Category="Settings")
	static UCartoUserSettings* Get();

	UFUNCTION(BlueprintCallable, Category="Settings")
	void SaveAndApply();
};
