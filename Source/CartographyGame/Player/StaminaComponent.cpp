// Copyright (c) 2026 The Last Cartographer.

#include "Player/StaminaComponent.h"

UStaminaComponent::UStaminaComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UStaminaComponent::Drain(float Amount)
{
	Current = FMath::Clamp(Current - FMath::Max(0.f, Amount), 0.f, Max);
	OnStaminaChanged.Broadcast(Get01());
}

void UStaminaComponent::Restore(float Amount)
{
	Current = FMath::Clamp(Current + FMath::Max(0.f, Amount), 0.f, Max);
	OnStaminaChanged.Broadcast(Get01());
}

void UStaminaComponent::TickStamina(float Dt, bool bJogging, bool bResting)
{
	if (bResting)      { Current = FMath::Min(Max, Current + RegenPerSecondResting * Dt); }
	else if (bJogging) { Current = FMath::Max(0.f, Current - DrainPerSecondJogging * Dt); }
	else               { Current = FMath::Min(Max, Current + RegenPerSecondWalking * Dt); }
	OnStaminaChanged.Broadcast(Get01());
}
