# System 5 — Ground Truth & Map Comparison

This is the hardest system. The goal is a hidden render target that
encodes the world's true categories per pixel, then a per-cell
comparison against the master map.

## 1. Category encoding

We use the channels of the ground-truth render target as feature flags:

| Channel | Meaning           |
|---------|-------------------|
| **R**   | Land              |
| **G**   | Water             |
| **B**   | Mountain peaks    |
| **A**   | Special features  |

The player's master map will, by virtue of pen colour and stroke
density, broadly approximate these. Tune `M_PencilBrush` /
`M_InkBrush` colours so accurate strokes land near these channels —
e.g. green-tinged ink for water, brown for land, blue for peaks. (Or
later, when watercolours unlock, use those colours directly.)

## 2. Ground-truth post-process material `M_PP_GroundTruth`

1. `Content/Materials/`, **Material** → `M_PP_GroundTruth`.
2. Material Domain: **Post Process**, Blendable Location: **Before
   Tonemapping**.
3. Sample `Scene Texture: World Normal` and `Scene Texture: Scene
   Depth` and `Scene Texture: Base Color`.
4. Build a per-pixel classification:
   - **Water**: `BaseColor` close to your water material's albedo
     OR `WorldNormal.Z > 0.99` and `WorldPositionZ < SeaLevel`.
   - **Land**: anything else with `WorldNormal.Z > 0.6`.
   - **Peaks**: `WorldPositionZ > PeakThreshold`.
   - **Special**: drive from a "Custom Stencil" value: any actor in
     the world with custom-stencil = 200 contributes to A.
5. Final colour: assemble `(IsLand, IsWater, IsPeak, IsSpecial)` into
   RGBA. Connect to `Emissive Color` (Output node).
6. Save.

## 3. Place `BP_GroundTruthCapture`

1. `Content/Blueprints/`, **Blueprint Class** parent `GroundTruthCapture`
   → `BP_GroundTruthCapture`.
2. Open it. Assign `Classification PP Material` = `M_PP_GroundTruth`.
3. Drop one in the level **directly above the centre of the island**,
   100 m up, rotation `(0, -90, 0)` (looking straight down).
4. Set `Ortho Width` to your island's larger dimension in cm.
5. Either create `RT_GroundTruth` (2048×2048) and assign it, or let
   `BeginPlay` create a transient one.

## 4. Run the comparison

Add a `MapComparisonComponent` to `BP_CartographyGameMode` (or to the
player). In the inking-desk "Done" Blueprint:

1. Get `BP_GroundTruthCapture` (find by class), call **Capture Now**.
2. `MapComparisonComponent → Run Comparison` with
   `MasterMapRT` from `Player.FieldMap` and the GT RT.
3. The component fills `LastResults` and broadcasts
   `OnComparisonComplete`.

The C++ tunables (`AccurateThreshold`, `InaccurateThreshold`,
`BlankLuminanceTolerance`) are exposed on the component instance —
expect to spend a few hours tuning them to feel right.

## 5. Push results to the world

Wire `OnComparisonComplete` to:

```
Get World → Get Subsystem (WorldResponseSubsystem) →
  Apply Comparison Results (Results)
```

That triggers `OnCellAccuracyChanged` for every cell whose state
flipped, which the next system uses.
