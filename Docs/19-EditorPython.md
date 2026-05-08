# System 19 — Editor Python auto-setup

`Tools/setup_project.py` runs **inside** the UE5 editor and creates the
binary `.uasset` infrastructure that the C++ scaffold expects, so you
don't have to click through hundreds of Content Browser actions.

## Prerequisites

- UE 5.4 open with `CartographyGame.uproject` loaded.
- Plugins enabled (already in the .uproject):
  - `PythonScriptPlugin`
  - `EditorScriptingUtilities`

## Running it

1. **Window → Output Log**.
2. Bottom-left filter dropdown: switch from **Cmd** to **Python**.
3. Type and run:
   ```
   py "Tools/setup_project.py"
   ```
4. The log will print every asset created. The script is idempotent —
   re-running it skips anything that already exists.

## What it creates

| Category               | Assets                                                        |
|------------------------|---------------------------------------------------------------|
| Folders                | Maps, Blueprints, UI, Materials, RenderTargets, Input, Pins, Landmarks, Audio, Data, MPC |
| Render targets (2048²) | `RT_FieldMap`, `RT_MasterMap`, `RT_GroundTruth`               |
| Sound classes          | `SC_Master`, `SC_Music`, `SC_Ambient`, `SC_SFX`               |
| MPC                    | `MPC_World` with TimeOfDay/Weather/Wind/Glitch parameters     |
| Input Actions          | IA_Move, IA_Look, IA_Jump, IA_ToggleFieldMap, IA_ToggleNotebook, IA_Interact, IA_Draw, IA_Pause, IA_QuickSave |
| Mapping context        | `IMC_Default` with key bindings                                |
| Blueprints             | BP_Cartographer, BP_GameMode, BP_PlayerController, BP_InkingDesk, BP_GroundTruthCapture, BP_FastTravelStone, BP_HiddenLandmark, BP_MapPin, BP_Wildlife, BP_ProceduralIsland |
| Data tables            | DT_Regions, DT_Biomes, DT_Tools, DT_Landmarks, DT_Lore, DT_Achievements (imported from `Content/Data/*.csv`) |
| Defaults               | Render targets wired into `BP_Cartographer.FieldMap`. Tables wired into `BP_GameMode`. GroundTruthRT wired into capture BP. Project default GameMode set to BP_GameMode. |

## What it cannot create

- **UMG widgets** (WBP_*) — their layouts have nuance Python can't safely
  produce. Build them by hand per `Docs/13-UI.md`. Total time: ~2 hours.
- **Materials** — same reason; node graphs are too rich. Build them per
  `Docs/02-FieldMap.md` and `Docs/05-Comparison.md`. Total time: ~3 hours.
- **WASD swizzle/negate modifiers on IMC_Default** — Python exposure for
  these is incomplete in 5.4. Open `IMC_Default`, click each WASD entry,
  add modifiers per `Docs/01-Player.md`. Total time: ~5 minutes.

## After running it

Follow this checklist in the editor:

1. Open `IMC_Default`. Add WASD swizzle/negate modifiers to `IA_Move`.
2. Build the materials in `Docs/02-FieldMap.md` and `Docs/05-Comparison.md`.
3. Open `BP_Cartographer`, assign the brush materials to the
   `FieldMap` component slots.
4. Build the UMG widgets in `Docs/13-UI.md`. Assign their classes on
   `BP_PlayerController`.
5. Drop a `BP_ProceduralIsland` into your `L_Island` map. Place
   `BP_GroundTruthCapture` 100m above it. Press Play.

You should be walking on a real island, with input + render targets +
data tables all working, in well under a day from clone.
