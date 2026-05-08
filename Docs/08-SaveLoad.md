# System 8 — Save / Load

`USaveLoadSubsystem` is a GameInstanceSubsystem with two main entry
points:

- `Save To Slot (SlotName, PlayerPawn, TimeOfDay) → bool`
- `Load From Slot (SlotName, PlayerPawn, OutTimeOfDay) → bool`

It writes:

- A `.sav` (Unreal `USaveGame` serialised) holding numeric/string state.
- Two PNGs next to it: `<Slot>_field.png`, `<Slot>_master.png` — the
  render targets exported via `UMapDrawingLibrary::SaveRenderTargetToPNG`.

## 1. UI hooks

Bind these to your pause menu buttons in a `WBP_PauseMenu`:

- **Save** →
  ```
  Get Game Instance → Get Subsystem (SaveLoadSubsystem) →
    Save To Slot ("Slot01", Player Pawn, Current Time Of Day)
  ```
- **Load** → same but `Load From Slot`. Pipe `OutTimeOfDay` back into
  your day-night system.

## 2. Autosave on inking-end

The inking-desk "Done" button is a natural autosave point. Call
`Save To Slot ("Auto", Player, TOD)` after the comparison runs.

## 3. Save location

Saves live under `Saved/SaveGames/<Slot>.sav`. The PNGs live under
`Saved/MapImages/<Slot>_(field|master).png`. Both directories are
inside the project's `Saved/` folder, ignored by git.

## 4. New-game vs continue

On the main menu:

- **Continue** is enabled iff
  `Save Load Subsystem → Does Save Exist ("Auto")` returns true.
- **New Game** opens `L_Island` with no slot to load.

## 5. Troubleshooting

- If the master map looks wrong after load, the PNG side-file is
  probably out of sync. Re-save and try again.
- The PNG load uses `KismetRenderingLibrary::ImportFileAsTexture2D`,
  which is editor-or-Saved-folder safe at runtime — keep saves under
  `Saved/`.
