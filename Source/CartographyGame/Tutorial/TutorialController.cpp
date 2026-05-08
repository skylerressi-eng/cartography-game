// Copyright (c) 2026 The Last Cartographer.

#include "Tutorial/TutorialController.h"

void UTutorialController::Notify(FName Event)
{
	if (CurrentStep == TEXT("OpenFieldMap")     && Event == TEXT("FieldMapOpened"))   { Advance(TEXT("DrawCoast")); }
	else if (CurrentStep == TEXT("DrawCoast")    && Event == TEXT("StrokeDrawn"))      { Advance(TEXT("AddPin")); }
	else if (CurrentStep == TEXT("AddPin")       && Event == TEXT("PinAdded"))         { Advance(TEXT("ReturnToCabin")); }
	else if (CurrentStep == TEXT("ReturnToCabin")&& Event == TEXT("EnteredCabin"))     { Advance(TEXT("InkMaster")); }
	else if (CurrentStep == TEXT("InkMaster")    && Event == TEXT("InkSessionDone"))   { Advance(TEXT("Sleep")); }
	else if (CurrentStep == TEXT("Sleep")        && Event == TEXT("Slept"))            { Advance(TEXT("Done")); }
}

void UTutorialController::Advance(FName Next)
{
	CurrentStep = Next;
	OnPrompt.Broadcast(CurrentStep, GetCurrentHint());
}

void UTutorialController::Skip() { CurrentStep = TEXT("Done"); }

FText UTutorialController::GetCurrentHint() const
{
	if (CurrentStep == TEXT("OpenFieldMap"))   return NSLOCTEXT("Tutorial", "Open",     "Press M to open your field map.");
	if (CurrentStep == TEXT("DrawCoast"))      return NSLOCTEXT("Tutorial", "Coast",    "Hold the left mouse button and trace the coast as you remember it.");
	if (CurrentStep == TEXT("AddPin"))         return NSLOCTEXT("Tutorial", "Pin",      "Press N. Note something you saw.");
	if (CurrentStep == TEXT("ReturnToCabin"))  return NSLOCTEXT("Tutorial", "Cabin",    "Make your way back to the cabin.");
	if (CurrentStep == TEXT("InkMaster"))      return NSLOCTEXT("Tutorial", "Ink",      "At the desk, press E. Ink your sketch onto the master map.");
	if (CurrentStep == TEXT("Sleep"))          return NSLOCTEXT("Tutorial", "Sleep",    "Rest until morning.");
	return FText::GetEmpty();
}
