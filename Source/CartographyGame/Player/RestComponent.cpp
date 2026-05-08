// Copyright (c) 2026 The Last Cartographer.

#include "Player/RestComponent.h"

#include "Map/FieldMapComponent.h"
#include "Player/CartographerCharacter.h"
#include "Player/StaminaComponent.h"
#include "Time/TimeOfDaySubsystem.h"

URestComponent::URestComponent() { PrimaryComponentTick.bCanEverTick = false; }

void URestComponent::BeginRestUntilMorning()
{
	UWorld* World = GetWorld();
	UTimeOfDaySubsystem* Tod = World ? World->GetSubsystem<UTimeOfDaySubsystem>() : nullptr;
	const float Pre = Tod ? Tod->CurrentHour : 0.f;
	if (Tod) { Tod->SleepUntilMorning(); }
	if (ACartographerCharacter* Cart = Cast<ACartographerCharacter>(GetOwner()))
	{
		if (Cart->FieldMap) { Cart->FieldMap->RefillInkToMax(); }
		if (UStaminaComponent* St = Cart->FindComponentByClass<UStaminaComponent>()) { St->Restore(1e6f); }
	}
	OnRested.Broadcast(Tod ? FMath::Fmod(24.f - Pre + Tod->CurrentHour, 24.f) : 8.f);
}

void URestComponent::BeginQuickRest(float Hours)
{
	UWorld* World = GetWorld();
	UTimeOfDaySubsystem* Tod = World ? World->GetSubsystem<UTimeOfDaySubsystem>() : nullptr;
	if (Tod) { Tod->SkipToHour(FMath::Fmod(Tod->CurrentHour + Hours, 24.f)); }
	if (ACartographerCharacter* Cart = Cast<ACartographerCharacter>(GetOwner()))
	{
		if (Cart->FieldMap) { Cart->FieldMap->RestoreInk(40.f); }
		if (UStaminaComponent* St = Cart->FindComponentByClass<UStaminaComponent>()) { St->Restore(50.f); }
	}
	OnRested.Broadcast(Hours);
}
