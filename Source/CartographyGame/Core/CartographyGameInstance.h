// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CartographyGameInstance.generated.h"

UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API UCartographyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable, Category="Game")
	void RequestNewGame();

	UFUNCTION(BlueprintCallable, Category="Game")
	void RequestContinue();
};
