# Pirate Islands import source

This folder holds the source asset used as **the game world** for The
Last Cartographer.

## Files

- `scene.gltf` — glTF JSON manifest (committed, 5.7 MB)
- `textures/*.png` — base-color textures (committed)
- `license.txt` — required attribution (CC-BY-4.0)
- **`scene.bin`** — binary mesh buffer (132 MB; **NOT in git** because
  GitHub's hard per-file limit is 100 MB and this repo doesn't use LFS).

## Getting `scene.bin`

After cloning, grab the original zip you uploaded
(`chicken_gun_pirateislands_reupload.zip`), unzip, and copy
`scene.bin` into this folder. The glTF has a relative reference to it
and won't import without it.

```
cartography-game/Content/ImportSource/PirateIslands/
├── scene.gltf
├── scene.bin     ← put it here
├── license.txt
└── textures/
    ├── PolygonPirates_Material_01_A_baseColor.png
    ├── PolygonPirates_Material_02_A_baseColor.png
    ├── PolygonPirates_Material_03_A_baseColor.png
    ├── PolygonPirates_Material_04_A_baseColor.png
    └── Water_baseColor.png
```

## Importing into UE5

In the editor: **Window → Output Log → Python**:

```
py "Tools/import_pirate_islands.py"
```

That script imports the glTF into `/Game/Maps/Imported/PirateIslands`,
makes a `BP_PirateIslandWorld` actor, and ticks the building-hider
component on so all `SM_Bld_*` and `SM_Prop_*` sub-meshes start
hidden — which is what you asked for ("remove the buildings"). You
can flip them back on individually in the Details panel later if you
want a particular ruin to remain.

## Credit (required by CC-BY-4.0)

This work is based on **"chicken gun pirateislands reupload"**
(<https://sketchfab.com/3d-models/chicken-gun-pirateislands-reupload-893581df1abf4d35a0dd849b169722fc>)
by **amogusstrikesback2**
(<https://sketchfab.com/amogusstrikesback2>) licensed under
**CC-BY-4.0** (<http://creativecommons.org/licenses/by/4.0/>).

Keep the same credit in your game's credits screen and Steam page.
