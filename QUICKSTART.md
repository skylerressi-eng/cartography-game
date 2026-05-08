# Quickstart — clone to playable

Three actions, one Python command, then **Play**.

## 0. Once-per-machine prerequisites

- **Unreal Engine 5.4** via the Epic Games Launcher.
- **Visual Studio 2022** with the *Game development with C++* workload
  (Windows). On macOS, Xcode.
- The original `chicken_gun_pirateislands_reupload.zip` you uploaded —
  you'll grab `scene.bin` from inside it.

## 1. Clone the repo

```
git clone https://github.com/skylerressi-eng/cartography-game.git
cd cartography-game
```

## 2. Drop in `scene.bin`

Unzip `chicken_gun_pirateislands_reupload.zip`, copy **only**
`scene.bin` into:

```
cartography-game/Content/ImportSource/PirateIslands/scene.bin
```

(`scene.gltf`, the textures and the license are already in git.
`scene.bin` is too big and stays out of git per `Docs/20-Map.md`.)

## 3. Generate VS project files + build

Right-click `CartographyGame.uproject` →
**Generate Visual Studio project files**.

Open `CartographyGame.sln`. Set:
- Configuration = **Development Editor**
- Platform = **Win64**

Press **Ctrl+Shift+B**. Wait ~3-5 minutes for the first build.

## 4. Open the project + run the bootstrap

Double-click `CartographyGame.uproject`. The editor compiles, opens
to an engine template map (intentional — `L_Island` doesn't exist
on a fresh clone yet).

In the editor:

1. **Window → Output Log**.
2. Bottom-left dropdown: switch from **Cmd** to **Python**.
3. Run:
   ```
   py "Tools/bootstrap.py"
   ```

That single command does **everything**:

- Creates folders, render targets, sound classes, MPC.
- Creates input actions and `IMC_Default` **with the WASD/swizzle/
  negate modifiers** so movement actually works.
- Creates every Blueprint subclass (`BP_Cartographer`, `BP_GameMode`,
  `BP_PlayerController`, `BP_WorldHider`, …).
- Builds the three brush materials (`M_PencilBrush`, `M_EraserBrush`,
  `M_InkBrush`) with the right node graph and parameter exposure.
- Imports the six DataTable CSVs.
- Creates UMG widget stubs (`WBP_HUD`, `WBP_FieldMap`, `WBP_Notebook`,
  `WBP_Inking`, `WBP_Pause`, `WBP_Compass`, `WBP_DiscoveryToast`)
  and wires them to `BP_PlayerController`.
- Creates the `L_Island` level and populates it: directional sun
  light tagged "SunLight", sky atmosphere, sky light, exponential
  height fog, volumetric cloud, water plane, player start, and
  a `BP_WorldHider` (so buildings auto-hide on play).
- Imports `scene.gltf` and spawns the islands at the origin.
- Sets project-default GameMode and startup map.

Takes about a minute end-to-end. Idempotent — safe to re-run.

## 5. Press Play

Click **Play** (or Alt+P). You should walk on the pirate islands
with WASD, look with the mouse, jump with Space, press M / N / E / LMB
for the field map / notebook / interact / draw bindings.

---

## What still benefits from polish (not blocking play)

The bootstrap creates **structurally valid but visually empty** UMG
widgets. The game runs and doesn't crash, but field map / notebook /
inking screens won't have nice layouts until you open each one and
add child widgets per `Docs/13-UI.md`.

Specifically:

- `WBP_HUD` — add `Stamina_Bar` (Progress Bar), `Ink_Bar` (Progress
  Bar), `Time_Text` / `Region_Text` / `Prompt_Text` / `TutorialHint_Text`
  (Text Blocks). The C++ `UHUDWidget::NativeTick` populates them
  automatically once they exist with those exact names.
- `WBP_Compass` — add a single `Arrow_Image`.
- `WBP_DiscoveryToast` — add `Title_Text` / `Subtitle_Text`.
- `WBP_FieldMap` / `WBP_Notebook` / `WBP_Inking` — assemble the
  layouts described in `Docs/02-FieldMap.md` / `Docs/03-Notebook.md`
  / `Docs/04-Inking.md`.

Each is roughly 10-30 minutes in the editor.

---

## Troubleshooting

| Symptom                                | Knob                                                   |
|----------------------------------------|--------------------------------------------------------|
| Editor opens to a black void           | Re-run `py "Tools/bootstrap.py"`. It's idempotent.    |
| WASD doesn't move character            | Open `IMC_Default`, verify W/A/S/D have `IA_Move` mappings + Swizzle/Negate modifiers. |
| Buildings still visible after Play     | Confirm `BP_WorldHider` exists in level outliner. Open it; Hider component `bApplyOnBeginPlay = true`. |
| Pin/landmark positions look off        | `BP_GameMode.WorldOriginXY/SizeXY` — re-run bootstrap to refresh from imported bounds. |
| Map drawing draws nothing              | Open `M_PencilBrush`. Confirm node graph exists. If empty, delete and re-run bootstrap. |
| Crash on Play with null UMG            | Open `WBP_HUD` etc; add the named child widgets above. |

If you get stuck, every system has its own doc (`Docs/00-Setup.md`
through `Docs/20-Map.md`), and `Docs/Architecture.md` explains how
they fit together.
