# System 6 — World Response

`UWorldResponseSubsystem` is a **World Subsystem** — auto-instantiated,
one per level. It holds a `GridSize × GridSize` array of
`ECellAccuracy`.

## 1. Configure the grid

In `BP_CartographyGameMode → BeginPlay`:

```
Get World → Get Subsystem (WorldResponseSubsystem) →
  Set Grid Config(
    Origin XY  = (-50000, -50000),
    Size XY    = (100000, 100000),
    Grid Size  = 32)
```

Use the **same** numbers as the GT capture's coverage and the
`FieldNotebook`'s world bounds.

## 2. Hidden landmarks

1. `Content/Landmarks/`, **Blueprint Class** parent `HiddenLandmark` →
   `BP_HiddenShrine` (or `BP_HiddenSpring`, etc.).
2. Set the static mesh and `MinAccuracyToReveal` =
   `AccuratelyMapped` (or `InaccuratelyMapped` for things that show
   even at partial accuracy).
3. Place instances around the island wherever lore-meaningful spots are.
4. At BeginPlay each landmark binds to the subsystem and hides itself
   if its cell isn't accurate — this is automatic.

## 3. Weather & fast-travel responses

In your weather Blueprint (`BP_WeatherManager`):

- On Tick (1 Hz is fine), poll
  `WorldResponseSubsystem → Get Accuracy At World (Player Location)`.
- If `AccuratelyMapped` → ramp weather toward the forecast.
- If `InaccuratelyMapped` → introduce subtle glitches: small camera
  drift, occasional rain bursts not in the forecast, wind that
  doesn't match cloud direction.
- If `Unmapped` → full unpredictability.

For fast travel, only allow waypoints on cells with accuracy
`AccuratelyMapped`. Querying is one node; the UI design is up to you.

## 4. Subtle "off" effects on inaccurate cells

A cheap implementation:

- `BP_CartographerCharacter → Tick`:
  - `Get Accuracy At World (Self.Location)` → if Inaccurate, gently
    add a sub-degree pitch wobble to the camera and alter character
    speed by ±5%. Keep it below a player's conscious threshold.

## 5. Test loop

1. Start map.
2. Walk + sketch + ink one corner accurately, leave another untouched.
3. Watch hidden landmarks appear in the inked corner after the
   inking session ends.
4. Walk into the un-inked corner — weather should feel "wrong".
