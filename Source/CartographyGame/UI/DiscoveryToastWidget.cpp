// Copyright (c) 2026 The Last Cartographer.

#include "UI/DiscoveryToastWidget.h"

#include "Components/TextBlock.h"
#include "TimerManager.h"

void UDiscoveryToastWidget::Show(const FText& Title, const FText& Subtitle, float DurationSeconds)
{
	if (Title_Text)    { Title_Text->SetText(Title); }
	if (Subtitle_Text) { Subtitle_Text->SetText(Subtitle); }
	if (!IsInViewport()) { AddToViewport(50); }
	OnShow();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HideTimer);
		World->GetTimerManager().SetTimer(HideTimer, this, &UDiscoveryToastWidget::HideNow, DurationSeconds, false);
	}
}

void UDiscoveryToastWidget::HideNow()
{
	OnHide();
	RemoveFromParent();
}
