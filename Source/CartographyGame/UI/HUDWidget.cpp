// Copyright (c) 2026 The Last Cartographer.

#include "UI/HUDWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Map/FieldMapComponent.h"
#include "Player/CartographerCharacter.h"
#include "Player/InteractionComponent.h"
#include "Player/StaminaComponent.h"
#include "Time/TimeOfDaySubsystem.h"
#include "Tutorial/TutorialController.h"
#include "World/BiomeManager.h"

void UHUDWidget::NativeTick(const FGeometry& Geo, float Dt)
{
	Super::NativeTick(Geo, Dt);

	APawn* Pawn = GetOwningPlayerPawn();
	ACartographerCharacter* Cart = Cast<ACartographerCharacter>(Pawn);
	if (!Cart) { return; }

	if (Stamina_Bar)
	{
		if (UStaminaComponent* St = Cart->FindComponentByClass<UStaminaComponent>())
		{
			Stamina_Bar->SetPercent(St->Get01());
		}
	}
	if (Ink_Bar && Cart->FieldMap)
	{
		Ink_Bar->SetPercent(Cart->FieldMap->MaxInk > 0.f ? Cart->FieldMap->CurrentInk / Cart->FieldMap->MaxInk : 0.f);
	}
	if (UWorld* World = GetWorld())
	{
		if (Time_Text)
		{
			if (UTimeOfDaySubsystem* T = World->GetSubsystem<UTimeOfDaySubsystem>())
			{
				const int32 Hours = FMath::FloorToInt(T->CurrentHour);
				const int32 Mins  = FMath::FloorToInt(FMath::Frac(T->CurrentHour) * 60.f);
				Time_Text->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Hours, Mins)));
			}
		}
		if (Region_Text)
		{
			if (UBiomeManager* B = World->GetSubsystem<UBiomeManager>())
			{
				const FRegionRow R = B->GetRegionRow(B->CurrentRegionId);
				Region_Text->SetText(R.DisplayName);
			}
		}
	}
	if (Prompt_Text)
	{
		if (UInteractionComponent* I = Cart->FindComponentByClass<UInteractionComponent>())
		{
			Prompt_Text->SetText(I->CurrentPrompt);
		}
	}
	if (TutorialHint_Text)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UTutorialController* Tc = GI->GetSubsystem<UTutorialController>())
			{
				TutorialHint_Text->SetText(Tc->GetCurrentHint());
			}
		}
	}
}
