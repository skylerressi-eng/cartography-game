// Copyright (c) 2026 The Last Cartographer.

#include "Core/CartographyPlayerController.h"

#include "Blueprint/UserWidget.h"

void ACartographyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HUDClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(this, HUDClass);
		if (HUDWidget) { HUDWidget->AddToViewport(0); }
	}
}

void ACartographyPlayerController::EnterUIMode(UUserWidget* Focus)
{
	if (!Focus) { return; }
	if (!Focus->IsInViewport()) { Focus->AddToViewport(10); }
	bShowMouseCursor = true;
	FInputModeGameAndUI Mode;
	Mode.SetWidgetToFocus(Focus->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
	SetInputMode(Mode);
}

void ACartographyPlayerController::LeaveUIMode()
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ACartographyPlayerController::ToggleFieldMap()
{
	if (!FieldMapClass) { return; }
	if (!FieldMapWidget) { FieldMapWidget = CreateWidget<UUserWidget>(this, FieldMapClass); }
	if (!FieldMapWidget) { return; }
	if (FieldMapWidget->IsInViewport()) { FieldMapWidget->RemoveFromParent(); LeaveUIMode(); }
	else                                 { EnterUIMode(FieldMapWidget); }
}

void ACartographyPlayerController::ToggleNotebook()
{
	if (!NotebookClass) { return; }
	if (!NotebookWidget) { NotebookWidget = CreateWidget<UUserWidget>(this, NotebookClass); }
	if (!NotebookWidget) { return; }
	if (NotebookWidget->IsInViewport()) { NotebookWidget->RemoveFromParent(); LeaveUIMode(); }
	else                                 { EnterUIMode(NotebookWidget); }
}

void ACartographyPlayerController::OpenInking()
{
	if (!InkingClass) { return; }
	if (!InkingWidget) { InkingWidget = CreateWidget<UUserWidget>(this, InkingClass); }
	if (!InkingWidget) { return; }
	EnterUIMode(InkingWidget);
}

void ACartographyPlayerController::CloseInking()
{
	if (InkingWidget && InkingWidget->IsInViewport()) { InkingWidget->RemoveFromParent(); }
	LeaveUIMode();
}

void ACartographyPlayerController::TogglePauseMenu()
{
	if (!PauseMenuClass) { return; }
	if (!PauseMenuWidget) { PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuClass); }
	if (!PauseMenuWidget) { return; }
	if (PauseMenuWidget->IsInViewport()) { PauseMenuWidget->RemoveFromParent(); LeaveUIMode(); }
	else                                  { EnterUIMode(PauseMenuWidget); }
}
