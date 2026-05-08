# System 10 — Weather

`UWeatherSubsystem` blends `Current` toward `Target`. Niagara/PP volumes
read `Current` for visuals.

## Setup

1. Create a `BP_WeatherDriver` actor placed once in the level. On
   BeginPlay, get the subsystem, call `RandomiseForecast(Seed=42)`.
2. On a 5-minute timer, call `RandomiseForecast(0)` to generate the
   next forecast.
3. Make a `BP_WeatherFX` actor that reads `Current.Intensity` and
   `Current.Kind`, spawning Niagara rain/storm systems and toggling
   exponential height fog density.

## Coupling to accuracy

In `BP_Cartographer → Tick` (or a low-frequency timer), call:

```
Get Subsystem (WorldResponseSubsystem) → Get Accuracy At World (Self.Location)
```

Map result to `Glitch01`:

| Accuracy            | Glitch |
|---------------------|--------|
| AccuratelyMapped    | 0.0    |
| InaccuratelyMapped  | 0.4    |
| Unmapped            | 0.7    |

Then call `WeatherSubsystem.ApplyAccuracyGlitch(Glitch01)`. Your
visual driver reads `Current.Glitch` and applies micro-effects: tiny
pitch wobble on the camera, occasional Niagara raindrops in clear
weather, a brief whisper sound.

Keep these effects below the player's conscious threshold — the game
shouldn't *show* the score, just *feel* off.
