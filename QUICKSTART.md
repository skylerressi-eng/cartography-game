# Quickstart — clone to playable in ~15 minutes

## 0. Prerequisites

- **Unreal Engine 5.4** (or newer 5.x) installed via the Epic Games Launcher
- **Visual Studio 2022** with the *Game development with C++* workload
  (Windows). On macOS, Xcode.
- The original `chicken_gun_pirateislands_reupload.zip` you uploaded
  (you'll need `scene.bin` from inside it — too big for git).

## 1. Clone the repo

```
git clone https://github.com/skylerressi-eng/cartography-game.git
cd cartography-game
```

## 2. Drop in `scene.bin`

Unzip your original pirate islands zip and copy **only** `scene.bin`
into:

```
cartography-game/Content/ImportSource/PirateIslands/scene.bin
```

(The folder already has `scene.gltf`, the textures, and the license.
`scene.bin` is git-ignored — see `Docs/20-Map.md`.)

## 3. Generate VS project files

Right-click `CartographyGame.uproject` →
**Generate Visual Studio project files**.

(macOS: `GenerateProjectFiles.command` from the engine install.)

## 4. Build the C++ module

Open the generated `CartographyGame.sln`. Set:
- Configuration = **Development Editor**
- Platform = **Win64**

Press **Ctrl+Shift+B** (Build → Build Solution). Wait ~3-5 minutes.

## 5. Open the project + run setup

Double-click `CartographyGame.uproject`. The editor compiles and opens
to an empty engine template map (intentional — `L_Island` doesn't
exist yet on a fresh clone).

In the editor:

1. **Window → Output Log**.
2. Bottom-left dropdown: switch from **Cmd** to **Python**.
3. Run:
   ```
   py "Tools/setup_project.py"
   ```

This creates `L_Island.umap`, all render targets, all input actions,
all Blueprint subclasses (BP_Cartographer, BP_GameMode, BP_WorldHider, …),
imports the six DataTable CSVs, and wires defaults across them. Takes
~30 seconds. Idempotent.

## 6. Import the islands

```
py "Tools/import_pirate_islands.py"
```

This loads `L_Island`, imports `scene.gltf`, spawns the imported scene
actor at the origin, and drops a `BP_WorldHider` so the buildings
auto-hide on play.

## 7. Press Play

Click **Play** (or Alt+P). You should walk on the pirate islands.

---

## What still needs human hands

Three things the script can't safely automate. Each is a one-shot
~5–30 minute job:

1. **WASD modifiers on `IMC_Default`** — open the asset, add Swizzle
   Input Axis Values + Negate to the WASD mappings on `IA_Move`.
   See `Docs/01-Player.md`.
2. **Brush materials** — `M_PencilBrush`, `M_EraserBrush`, `M_InkBrush`
   per `Docs/02-FieldMap.md`.
3. **UMG widgets** — the field map, notebook, inking, HUD widgets per
   `Docs/13-UI.md`. The C++ base classes are done; you build the
   layouts in BP.

Until you do (1), the player won't move. Until (2), drawing won't
render. Until (3), no UI shows.

---

## Things you can flip if something breaks

| Symptom                                | Knob                                                    |
|----------------------------------------|---------------------------------------------------------|
| Editor opens to a black void           | Run `setup_project.py` again — should create L_Island. |
| Buildings still visible after Play     | Check the spawned `BP_WorldHider` Hider component's `bApplyOnBeginPlay = true`. |
| Buildings *not* visible at all in editor | Toggle `bReveal = true`, call `Apply` from the Outliner. |
| Pin/landmark positions look wrong       | `BP_GameMode.WorldOriginXY`/`WorldSizeXY` — match to the imported scene's bounds (run `Recompute Island Bounds` from the actor). |
| Game crashes on Play                    | `Output Log` will tell you which UMG/material asset is null. Most "must-create" assets are listed in step 5/6 above. |

If you get stuck, every system has its own doc:
`Docs/00-Setup.md` … `Docs/20-Map.md`.
