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

## What you get out of the box

The bootstrap fills in genuine UMG layouts, not stubs:

- `WBP_HUD`: stamina + ink bars top-left, time + region top-right,
  interaction prompt + tutorial hint bottom-centre. The C++ NativeTick
  populates them every frame.
- `WBP_FieldMap`: full-screen parchment with the live `RT_FieldMap`
  render target showing your pencil strokes.
- `WBP_Inking`: side-by-side field map + master map render targets.
- `WBP_Notebook`: parchment-tinted scroll view ready for entries.
- `WBP_Pause`: dim overlay with Resume / Save / Load / Settings / Quit.
- `WBP_Compass`, `WBP_DiscoveryToast`: minimal but live.

The character also has working **C++ default behaviour** for every
input — no Blueprint graph wiring needed:

- M / N → toggle Field Map / Notebook (calls into `PlayerController`).
- E → if near an `InkingDesk`, opens inking; otherwise interacts.
- LMB while map is open → strokes a pencil onto the field map every
  tick at the current mouse UV.

## What still benefits from polish

Mostly visual — none of it blocks play.

- The widget layouts are functional but austere. Open them in the
  editor to add fonts, parchment textures, a hand-drawn frame.
- The brush materials use a procedural circular falloff. Replace
  with a hand-painted alpha mask for stylised pencil/ink.
- `L_Island` ships with engine-default sky and a flat water plane.
  Replace with the Water plugin's lake mesh + a stylised post-process
  for the painterly look.

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
