# Setup — bringing the project up the first time

You need **Unreal Engine 5.4** (or newer 5.x) on Windows or macOS, plus a
C++ toolchain (Visual Studio 2022 with the *Game development with C++*
workload on Windows, or Xcode on macOS).

## 1. Generate the project files

- Right-click `CartographyGame.uproject` → **Generate Visual Studio project files**
  (or run `GenerateProjectFiles.bat` on Windows / `GenerateProjectFiles.sh`
  on macOS from the engine install).
- Open `CartographyGame.sln`. Set configuration **Development Editor**,
  platform **Win64**. Build (Ctrl+Shift+B).

## 2. First open

- Double-click `CartographyGame.uproject`. The editor compiles the
  C++ module and opens.
- If it offers to "rebuild missing modules", click **Yes**.

## 3. Top-level Content folders

Create these in the **Content Browser** (right-click in `Content/`):

```
Content/
  Maps/                  # Levels
  Blueprints/            # BP_Cartographer, BP_GameMode, etc.
  UI/                    # Widget Blueprints
  Materials/             # Brush materials, ground-truth PP
  RenderTargets/         # FieldMapRT, MasterMapRT, GroundTruthRT
  Input/                 # Input Mapping Context + Input Actions
  Pins/                  # MapPin Blueprints + meshes
  Landmarks/             # HiddenLandmark Blueprints
```

## 4. Game mode

1. Right-click `Content/Blueprints/` → **Blueprint Class** → search **Game Mode Base** → name `BP_CartographyGameMode`.
2. Open it. Set **Default Pawn Class** = `BP_Cartographer` (you'll create
   that in `Docs/01-Player.md`).
3. Open **Project Settings → Maps & Modes**. Set **Default GameMode** = `BP_CartographyGameMode`.

Continue to `Docs/01-Player.md`.
