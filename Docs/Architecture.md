# Architecture overview

```
                    +------------------+
                    |  BP_GameMode     |
                    +---------+--------+
                              |
                              v
+-----------------+   +-------+----------+   +---------------------+
| InputContexts   +-->| BP_Cartographer  +-->| FieldMapComponent   |
| IMC_Default     |   | (ACharacter)     |   |  - RT_FieldMap      |
+-----------------+   +-------+----------+   |  - RT_MasterMap     |
                              |              |  - Ink              |
                              v              +---------+-----------+
                      +-------+-------+                |
                      | Notebook      |                v
                      | + MapPins     |       +--------+----------+
                      +---------------+       | MapDrawingLibrary |
                                              | (BeginDrawCanvas) |
                                              +-------------------+

  At the cabin desk
  -----------------
  AInkingDesk → opens WBP_Inking → uses MasterMap brush ("Ink")
                  ↓ on Done
  AGroundTruthCapture::CaptureNow()
                  ↓
  UMapComparisonComponent::RunComparison(Master, GT)
                  ↓ OnComparisonComplete
  UWorldResponseSubsystem::ApplyComparisonResults
                  ↓ OnCellAccuracyChanged(per cell)
  - AHiddenLandmark.ApplyVisibility
  - WeatherManager.AdjustForCell
  - FastTravel.EnableWaypointForCell
```

## Coordinate conventions

All XY world bounds use *centimetres* (UE units). Pick a set of
`(OriginXY, SizeXY)` values once and reuse them in:

- `UFieldNotebookComponent` (per-pin → UV projection)
- `AGroundTruthCapture` (`OrthoWidth`)
- `UWorldResponseSubsystem::SetGridConfig`
- Any custom UI that maps world → map UV

A 1 km² island centred on the origin → Origin `(-50000, -50000)`,
Size `(100000, 100000)`.

## Threading

- All map drawing happens on the game thread via Begin/Draw/End
  Canvas. UE marshals to RT internally — you don't manage that.
- `MapComparison.RunComparison` does a CPU readback of two render
  targets — call it once per inking session, **never per frame**.

## Subsystem registry

| Class                       | Scope          | Lifetime          |
|-----------------------------|----------------|-------------------|
| `UWorldResponseSubsystem`   | World          | per-level         |
| `UTimeOfDaySubsystem`       | World (tick)   | per-level         |
| `UWeatherSubsystem`         | World (tick)   | per-level         |
| `UMusicDirector`            | World (tick)   | per-level         |
| `UFastTravelSubsystem`      | World          | per-level         |
| `UBiomeManager`             | World          | per-level         |
| `UProgressionSubsystem`     | Game Instance  | session           |
| `ULoreSubsystem`            | Game Instance  | session           |
| `UDiscoveryLog`             | Game Instance  | session           |
| `UAchievementSubsystem`     | Game Instance  | session           |
| `UTutorialController`       | Game Instance  | session           |
| `USaveLoadSubsystem`        | Game Instance  | session           |
| `USaveSlotManager`          | Game Instance  | session           |

## Performance budget

- Render targets: 2× 2048² RGBA8 ≈ 32 MB GPU. Acceptable.
- GroundTruth RT: another 16 MB; capture once at startup.
- Comparison: ~4 MB readback × 2, ~1 ms of CPU work for 32×32 grid.
- HiddenLandmark Tick is disabled — purely event-driven.
