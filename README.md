# The Last Cartographer

A quiet, single-player exploration & cartography game in Unreal Engine 5.4+.

> **New here?** Open [`QUICKSTART.md`](QUICKSTART.md). It walks the
> exact bring-up: clone, drop in `scene.bin`, build C++, run **one**
> Python command, press Play.

## What's in this repository

This repo is a **C++ scaffold** for the systems described in the build plan.
It compiles into a working `CartographyGame` runtime module with classes
that cover all eight systems. The actual playable level — the island
landscape, materials, UI widgets, and Blueprint subclasses that wire those
C++ systems to input and gameplay — must be built inside the Unreal editor
on Windows or macOS. See `Docs/00-Setup.md` for the bring-up sequence.

## What got expanded

In addition to the eight original systems the scaffold now includes:

- **Day/night** subsystem (sun rotation, phase events) — `Time/`
- **Weather** subsystem (forecast blend, accuracy glitches) — `Weather/`
- **Audio mood + music director** — `Audio/`
- **Fast travel** markers and subsystem — `FastTravel/`
- **Stamina, rest, interaction** components — `Player/`
- **Region & biome** (data-driven) + trail recording + wildlife — `World/`
- **Symbol stamps, ruler/compass tools, bookmarks** — `Map/`
- **Lore + discovery + tutorial** subsystems — `Lore/`, `Discovery/`, `Tutorial/`
- **Achievements + user settings** — `Achievements/`, `Settings/`
- **Multi-slot save manager** with thumbnails — `Save/SaveSlotManager`
- **GameMode / PlayerController / HUD / Compass / Toast** base classes — `Core/`, `UI/`
- **Six DataTable CSVs** ready to import — `Content/Data/`

Read `Docs/00-Setup.md` first, then `Docs/01-Player.md` ... `Docs/17-Data.md`.

## Project layout

```
CartographyGame.uproject              # Project file
Config/                               # DefaultEngine/Game/Input.ini
Source/
  CartographyGame.Target.cs
  CartographyGameEditor.Target.cs
  CartographyGame/
    CartographyGame.{h,cpp}           # Module entry
    CartographyGame.Build.cs
    Player/CartographerCharacter      # System 1
    Map/FieldMapComponent             # System 2
    Map/MapDrawingLibrary
    Notebook/FieldNotebookComponent   # System 3
    Notebook/MapPin
    Inking/InkingDesk                 # System 4
    GroundTruth/GroundTruthCapture    # System 5
    GroundTruth/MapComparisonComponent
    World/WorldResponseSubsystem      # System 6
    World/HiddenLandmark
    Progression/ProgressionSubsystem  # System 7
    Save/CartographySaveGame          # System 8
    Save/SaveLoadSubsystem
Content/                              # Empty — populate from the editor
Docs/                                 # Step-by-step setup per system
```

## Bring-up checklist (first time on Windows/macOS)

1. Install Unreal Engine 5.4 (or newer 5.x).
2. Right-click `CartographyGame.uproject` → **Generate Visual Studio project files**.
3. Open the generated `.sln`, build the **Development Editor** target.
4. Open `CartographyGame.uproject`. The editor compiles the module.
5. **Window → Output Log → Python** dropdown → run:
   ```
   py "Tools/setup_project.py"
   ```
   This auto-creates render targets, input actions, BP subclasses, and
   imports all data tables. See `Docs/19-EditorPython.md`.
6. Drop a `BP_ProceduralIsland` into a new Open World level for instant
   walkable terrain — see `Docs/18-Procedural.md`.
7. Follow `Docs/00-Setup.md` through `Docs/19-EditorPython.md` to wire
   the rest (UMG widgets, materials, IMC swizzle modifiers).

## Game world: Pirate Islands

The default world is a CC-BY-4.0 Sketchfab scene by
**amogusstrikesback2** (see `Content/ImportSource/PirateIslands/license.txt`).
Buildings (`SM_Bld_*`) and props (`SM_Prop_*`) are hidden at runtime
by `UBuildingHiderComponent` so the player explores an uncharted
island feel — grass, palms, rocks, beaches, mangroves, clouds.

After cloning, drop `scene.bin` (from your original
`chicken_gun_pirateislands_reupload.zip`) into
`Content/ImportSource/PirateIslands/` (it's git-ignored — too big),
then in the UE5 editor:

```
py "Tools/import_pirate_islands.py"
```

See `Docs/20-Map.md` for the full flow, including how to keep
specific buildings visible.

This work is based on
[*chicken gun pirateislands reupload*](https://sketchfab.com/3d-models/chicken-gun-pirateislands-reupload-893581df1abf4d35a0dd849b169722fc)
by [amogusstrikesback2](https://sketchfab.com/amogusstrikesback2)
licensed under [CC-BY-4.0](http://creativecommons.org/licenses/by/4.0/).

## Design pillars

- Walking pace, no fast travel until earned through accurate mapping.
- Field map = pencil, erasable, no scoring.
- Master map (at the home desk) = ink, permanent, scored silently.
- The world *responds* to accuracy — never shows a number.
- One island, ~1 km², zero combat, zero NPCs (or one hermit, max).
