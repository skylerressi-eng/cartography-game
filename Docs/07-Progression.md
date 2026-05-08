# System 7 — Progression

`UProgressionSubsystem` is a `GameInstanceSubsystem` — survives level
loads, doesn't survive a fresh game launch (the SaveGame restores it).

## 1. Drive unlocks from inking sessions

Track an integer "regions inked" counter on the game state. After each
inking session that produces at least one new accurate region, call:

```
Get Game Instance → Get Subsystem (ProgressionSubsystem) →
  On Region Inked (NewRegionCount)
```

The C++ implementation unlocks `Compass`, `Elevation`, `Watercolor`,
`FastTravel` at counts 1, 3, 5, 8 — adjust to taste.

## 2. Tools as widgets

In `WBP_FieldMap` and `WBP_Inking`, gate optional tool buttons on:

```
Get Game Instance → Get Subsystem (ProgressionSubsystem) →
  Is Unlocked ("Watercolor")
```

Watercolour is just an alternate brush material with colour parameters
exposed (use `M_PencilBrush` as a base, swap `BrushColor` per tool).

## 3. Compass widget

When `Compass` unlocks, show a small north-arrow widget in the
top-right corner. Bind its rotation to
`Get Player Camera Manager → Get Camera Rotation → Yaw` (negated).

## 4. Notifications

Bind `OnToolUnlocked` to a small toast widget that fades in/out
showing the tool name. Don't over-celebrate — this game is quiet.
