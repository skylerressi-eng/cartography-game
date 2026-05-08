# System 14 — User Settings

`UCartoUserSettings` extends `UGameUserSettings` and is wired in
`Config/DefaultEngine.ini` via:

```
[/Script/Engine.GameEngine]
+GameUserSettingsClassName=/Script/CartographyGame.CartoUserSettings
```

That makes `UGameUserSettings::GetGameUserSettings()` return your
subclass. From BP/UI, call:

```
Get Carto User Settings → (modify) → Save And Apply
```

## Hooking volumes to audio

UE has Sound Class assets. Make four:

- `SC_Master`, `SC_Music`, `SC_Ambient`, `SC_SFX`

A lightweight way to drive volume: in `BP_AudioRig` Tick, read
`UCartoUserSettings::Get()` and call:

```
Set Sound Class Volume (SC_Music, MusicVolume * MasterVolume, 0.2)
```

## Mouse sensitivity / invert-Y

In `BP_Cartographer → Look`:
```
Final = Axis * MouseSensitivity, Y *= (bInvertY ? -1 : 1)
```
Override the C++ `ACartographerCharacter::Look` in BP if you prefer.
