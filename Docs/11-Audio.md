# System 11 — Audio (Mood & Music)

Two pieces:

- `UAudioMoodComponent` — N looping sound layers crossfaded by
  per-layer target volume (0..1).
- `UMusicDirector` — world subsystem that watches time/weather/
  accuracy and sets layer targets each tick.

## Layer naming convention

Hard-coded ids the director expects:

- `Calm`   — daytime ambient music
- `Drone`  — night drone
- `Wonder` — discovery / awe stinger
- `Storm`  — heavy weather
- `Sleep`  — sleeping at cabin

## Setup

1. Spawn a persistent `BP_AudioRig` actor at level start with one
   `AudioMoodComponent`. Populate `Layers` with one entry per id; set
   the `Sound` reference for each. Use looping ambient soundwaves.
2. On `BeginPlay`, in your `BP_GameMode` or game instance: assign
   `Get Subsystem (MusicDirector) → Mixer = BP_AudioRig.AudioMoodComponent`.
3. From any system that wants to celebrate quietly, call
   `Get Subsystem (MusicDirector) → Signal Discovery()` —
   the `Wonder` layer briefly swells then fades back.

## Sourcing audio

Don't compose this yourself. Buy a stylised pack on Marketplace
("Calm Folk Music Pack", etc.) or commission a small set on Fiverr.
Five 60-second loops will carry the entire game.
