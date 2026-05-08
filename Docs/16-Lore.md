# System 16 — Lore, Discovery & Tutorial

Three subsystems work together for environmental storytelling:

- `ULoreSubsystem` (game instance) — the lore *book*; rows from
  `DT_Lore`.
- `UDiscoveryLog` (game instance) — chronological list of named
  things the player has *seen*.
- `UTutorialController` (game instance) — first-hour state machine.

## Lore objects

Make a `BP_LoreNote` actor (parent: `Actor`, implements `Interactable`).
Properties:

- `Lore Id` (FName)
- Static mesh: a parchment scroll or scratched plaque

Implement `Interact`:

```
Get Subsystem (LoreSubsystem) → Mark Found (LoreId)
Get Subsystem (DiscoveryLog) → Record (LoreId, Title, "Lore", Loc)
Show toast → "Found a note"
Open WBP_LoreReader with the row's body text
```

Place these around the island per `DT_Landmarks.LoreEntryId`.

## Discovery vs lore

- *Discovery* is the broader concept (place, lore, landmark).
- *Lore* is text content the player can re-read in a journal.

Bind `UDiscoveryLog::OnDiscovery` → `MusicDirector.SignalDiscovery()`
+ `BP_DiscoveryToast.Show()`.

## Tutorial state machine

`UTutorialController::Notify(Event)` advances through:

```
OpenFieldMap  — wait for "FieldMapOpened"
DrawCoast     — wait for "StrokeDrawn"
AddPin        — wait for "PinAdded"
ReturnToCabin — wait for "EnteredCabin"
InkMaster     — wait for "InkSessionDone"
Sleep         — wait for "Slept"
Done
```

Notify hooks (call from BP):
- `BP_Cartographer.OnToggleFieldMap` → `Notify("FieldMapOpened")`
- `WBP_FieldMap` first stroke → `Notify("StrokeDrawn")`
- `Notebook.AddEntry` → `Notify("PinAdded")`
- `BP_InkingDesk.OnInkingBegan` → on **end** instead, after
  `MapComparison.RunComparison` → `Notify("InkSessionDone")`
- Cabin trigger volume overlap → `Notify("EnteredCabin")`
- `URestComponent.OnRested` → `Notify("Slept")`

Hint text is pulled by `BP_HUD.TutorialHint_Text` automatically each
frame; once `IsComplete()` returns true, the hint goes empty and the
tutorial silently retires.

## Hermit (optional NPC)

If you keep one NPC, model the hermit as a `BP_LoreNote` with a
walking idle. Three lore entries (`Hermit01`, `Hermit02`, `Hermit03`)
unlock as the player ink-completes more regions — gate via
`ProgressionSubsystem.IsUnlocked` in `Interact`.
