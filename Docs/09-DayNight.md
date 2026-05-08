# System 9 — Day/Night

`UTimeOfDaySubsystem` is a tickable world subsystem. It rotates a
directional light tagged `SunLight` to match `CurrentHour`, broadcasts
`OnHourChanged` and `OnPhaseChanged` (Dawn / Morning / Noon /
Afternoon / Dusk / Night).

## Setup

1. In your level, select your **Directional Light**, set **Mobility =
   Movable**, and add tag `SunLight` in the **Actor Tags** array.
2. (Optional) tweak `HoursPerSecond` in defaults — `1/60` means one
   in-game hour per real-world minute.
3. Place a `BP_TimeOfDayDriver` actor that on BeginPlay reads
   `Get World → Get Subsystem(TimeOfDaySubsystem)` and exposes the
   value to UMG/Sequencer.

## Use cases

- **Sleep at the cabin** — `URestComponent::BeginRestUntilMorning`
  calls `SleepUntilMorning`. Time jumps to 7:00.
- **Music director** — reads `GetPhase()` and biases ambient layers.
- **HUD clock** — bind `Time_Text` to `CurrentHour`.

## Lighting tip

`GetSunPitchDegrees` returns a sine curve from -5° at sunrise/sunset
to -85° at noon, and stays above the horizon at night so the SkyAtmosphere
keeps coloured properly. Override in your BP if you want a polar setting.
