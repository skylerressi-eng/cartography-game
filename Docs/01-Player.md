# System 1 — Player & World Foundation

The C++ class `ACartographerCharacter` already provides:

- `USpringArmComponent` + `UCameraComponent` (third-person)
- `UFieldMapComponent` and `UFieldNotebookComponent` as default subobjects
- Walk speed = 200 cm/s
- Enhanced Input bindings for Move, Look, Jump, Toggle Map (`M`),
  Toggle Notebook (`N`), Interact (`E`), Draw (LMB)

You configure inputs and the Blueprint subclass in the editor.

## 1. Create Input Actions

In `Content/Input/`, right-click → **Input → Input Action**:

| Asset                 | Value Type   | Notes                            |
|-----------------------|--------------|----------------------------------|
| `IA_Move`             | Axis2D       | WASD                             |
| `IA_Look`             | Axis2D       | Mouse XY                         |
| `IA_Jump`             | Digital Bool | Space                            |
| `IA_ToggleFieldMap`   | Digital Bool | M                                |
| `IA_ToggleNotebook`   | Digital Bool | N                                |
| `IA_Interact`         | Digital Bool | E                                |
| `IA_Draw`             | Digital Bool | Left Mouse Button                |

## 2. Create the Input Mapping Context

- Right-click → **Input → Input Mapping Context** → `IMC_Default`.
- Open it. Add the actions above with these mappings:

| Action               | Key   | Modifiers                                     |
|----------------------|-------|-----------------------------------------------|
| `IA_Move`            | W     | Swizzle Input Axis Values (YXZ)               |
| `IA_Move`            | S     | Negate, Swizzle Input Axis Values (YXZ)       |
| `IA_Move`            | A     | Negate                                        |
| `IA_Move`            | D     | (none)                                        |
| `IA_Look`            | Mouse XY | (none)                                     |
| `IA_Jump`            | Space Bar | (none)                                    |
| `IA_ToggleFieldMap`  | M     | (none)                                        |
| `IA_ToggleNotebook`  | N     | (none)                                        |
| `IA_Interact`        | E     | (none)                                        |
| `IA_Draw`            | Left Mouse Button | (none)                            |

## 3. Create `BP_Cartographer`

1. In `Content/Blueprints/`, right-click → **Blueprint Class** → search
   `CartographerCharacter` (parent class).
2. Name it `BP_Cartographer`.
3. Open it. In the **Class Defaults** panel, assign:
   - `Default Mapping Context` → `IMC_Default`
   - `Move Action` → `IA_Move`
   - `Look Action` → `IA_Look`
   - `Jump Action` → `IA_Jump`
   - `Toggle Field Map Action` → `IA_ToggleFieldMap`
   - `Toggle Notebook Action` → `IA_ToggleNotebook`
   - `Interact Action` → `IA_Interact`
   - `Draw Action` → `IA_Draw`
   - `Walk Speed` = `200`
4. Compile, save.

## 4. Set the game-mode pawn

`BP_CartographyGameMode → Default Pawn Class = BP_Cartographer` (you did
this in `Docs/00-Setup.md`).

## 5. Make a basic landscape

- **File → New Level → Open World** (Empty).
- Save as `Content/Maps/L_Island.umap`.
- **Window → Place Actors** → drag a **Landscape** in. Sculpt at low
  resolution (4×4 components) for now; you'll iterate later.
- Add a **Sky Atmosphere**, **Sun Light** (Directional Light, Movable),
  **Sky Light**, **Exponential Height Fog**, **Volumetric Cloud**,
  **Post Process Volume** (Unbound).
- Drop a **Player Start** somewhere on the landscape.

Press **Play**. You should walk at a slow pace. `M`, `N`, `E`, LMB do
nothing visible yet — wiring those is in the next docs.

## 6. (Optional) Day/night & weather

- Make `BP_TimeOfDay` (Actor) with a float `Hour` and a Timeline that
  rotates the directional light. Stub for now.
- Weather: use the **Niagara Rain** template or a particle system tied to
  a `WeatherIntensity` float. Used later by world-response.
