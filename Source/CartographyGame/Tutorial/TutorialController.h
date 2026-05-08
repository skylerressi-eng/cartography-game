// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TutorialController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTutorialPrompt, FName, StepId, FText, Hint);

/**
 * Drives the first hour. Each step is a one-shot trigger checked against
 * a state machine. The first step prompts the player to open the field
 * map; once they have, advance to "draw a coast"; and so on. Quietly
 * disables itself once complete.
 */
UCLASS()
class CARTOGRAPHYGAME_API UTutorialController : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Tutorial") FOnTutorialPrompt OnPrompt;
	UPROPERTY(BlueprintReadOnly,    Category="Tutorial") FName CurrentStep = TEXT("OpenFieldMap");

	UFUNCTION(BlueprintCallable, Category="Tutorial") void Notify(FName Event);
	UFUNCTION(BlueprintCallable, Category="Tutorial") void Skip();
	UFUNCTION(BlueprintCallable, Category="Tutorial") FText GetCurrentHint() const;
	UFUNCTION(BlueprintPure,    Category="Tutorial") bool IsComplete() const { return CurrentStep == TEXT("Done"); }

private:
	void Advance(FName NextStep);
};
