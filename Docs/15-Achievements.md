# System 15 — Achievements

`UAchievementSubsystem` is a game instance subsystem. Source of truth
is the `DT_Achievements` data table (CSV provided in `Content/Data/`).

## Wiring the data table

1. In editor, right-click `Content/Data/` → **Import** →
   `DT_Achievements.csv`. Choose **Data Table** with row struct
   `FAchievementRow`.
2. In `BP_GameMode` defaults, set `Achievements Table` to the imported
   asset.
3. `ACartographyGameMode::StartPlay` automatically pushes it into the
   subsystem.

## Triggering unlocks

Call from anywhere:

```
Get Subsystem (AchievementSubsystem) → Unlock("FirstStroke")
```

Bind `OnUnlocked` to a `BP_DiscoveryToast` instance to show a small
parchment-style notification.

## Suggested triggers

| Achievement      | Trigger                                                       |
|------------------|---------------------------------------------------------------|
| FirstStroke      | First call to `FieldMapComponent.DrawAt` with Pencil          |
| FirstInk         | First call with Ink brush                                      |
| FirstAccurate    | `OnComparisonComplete`: any `AccuratelyMapped` cell present   |
| AllRegions       | After comparison: each region has ≥ 1 accurate cell            |
| HiddenLight      | Discover Old Beacon during `Dawn` phase                       |
| QuietMile        | `TrailComponent` length ≥ 100k cm with no FieldMap open       |
| NotebookFull     | `Notebook.Entries.Num() >= 50`                                |

## Steam

When you reach Steam integration, swap calls to forward to
`ISteamUserStats::SetAchievement` after the local unlock.
