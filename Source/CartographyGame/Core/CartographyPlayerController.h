// Copyright (c) 2026 The Last Cartographer.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CartographyPlayerController.generated.h"

class UUserWidget;

UCLASS(Blueprintable)
class CARTOGRAPHYGAME_API ACartographyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<UUserWidget> HUDClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<UUserWidget> FieldMapClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<UUserWidget> NotebookClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<UUserWidget> InkingClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI") TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY(BlueprintReadOnly, Category="UI") TObjectPtr<UUserWidget> HUDWidget;
	UPROPERTY(BlueprintReadOnly, Category="UI") TObjectPtr<UUserWidget> FieldMapWidget;
	UPROPERTY(BlueprintReadOnly, Category="UI") TObjectPtr<UUserWidget> NotebookWidget;
	UPROPERTY(BlueprintReadOnly, Category="UI") TObjectPtr<UUserWidget> InkingWidget;
	UPROPERTY(BlueprintReadOnly, Category="UI") TObjectPtr<UUserWidget> PauseMenuWidget;

	UFUNCTION(BlueprintCallable, Category="UI") void ToggleFieldMap();
	UFUNCTION(BlueprintCallable, Category="UI") void ToggleNotebook();
	UFUNCTION(BlueprintCallable, Category="UI") void OpenInking();
	UFUNCTION(BlueprintCallable, Category="UI") void CloseInking();
	UFUNCTION(BlueprintCallable, Category="UI") void TogglePauseMenu();

protected:
	virtual void BeginPlay() override;

	void EnterUIMode(UUserWidget* Focus);
	void LeaveUIMode();
};
