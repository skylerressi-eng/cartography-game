# Data Tables

CSV templates live in `Content/Data/`. Import each in the editor:

1. Right-click in the Content Browser → **Import**.
2. Select the `.csv` file.
3. In the import dialog, **Data Table** type, **Row Struct** set per
   the table below.
4. Save the resulting `DT_*` asset.

| CSV                  | Row Struct                  |
|----------------------|-----------------------------|
| `DT_Regions.csv`     | `FRegionRow`                |
| `DT_Biomes.csv`      | `FBiomeRow`                 |
| `DT_Tools.csv`       | `FToolRow`                  |
| `DT_Landmarks.csv`   | `FLandmarkRow`              |
| `DT_Lore.csv`        | `FLoreRow`                  |
| `DT_Achievements.csv`| `FAchievementRow`           |

After import, assign the `DT_*` assets on `BP_CartographyGameMode`
defaults (`Regions Table`, `Biomes Table`, etc.). The C++ game mode
pushes them into the right subsystems on StartPlay.

## Editing in-place

UE's data-table editor lets you edit rows directly in the editor.
For larger edits, export back to CSV → edit → re-import. UE preserves
the row keys on re-import.

## Field formats reminder

- `FVector2D` → `(X=...,Y=...)` (no space)
- `FVector` → `(X=...,Y=...,Z=...)`
- `FLinearColor` → `(R=...,G=...,B=...,A=...)`
- `FName` (RegionId etc.) → bare token, no quotes
- `FText` (DisplayName, Body) → quoted; `\n` for newlines
