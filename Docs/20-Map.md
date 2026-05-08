# System 20 — Pirate Islands as the world

The default game map is the **Pirate Islands** scene from Sketchfab
(by amogusstrikesback2, CC-BY-4.0). The C++ glue + Python importer
make it a one-command install.

## Files (already in repo, except scene.bin)

```
Content/ImportSource/PirateIslands/
├── scene.gltf                 # in git
├── scene.bin                  # NOT in git (132 MB) — copy from your zip
├── license.txt
├── textures/                  # 5 base-colour textures, in git
└── README.md
```

`scene.bin` exceeds GitHub's 100 MB hard cap and the repo doesn't use
LFS. After cloning, drop `scene.bin` (from
`chicken_gun_pirateislands_reupload.zip`) next to `scene.gltf` and
you're set.

## Importing

In the editor:

```
py "Tools/import_pirate_islands.py"
```

The script:

1. Verifies `scene.bin` exists.
2. Imports the glTF into `/Game/Maps/Imported/PirateIslands` (a few
   thousand StaticMesh assets — preserves their `SM_Bld_*` /
   `SM_Env_*` / `SM_Prop_*` names).
3. Spawns a `BP_PirateIslandWorld` actor in the current level and
   attaches all imported meshes to it.
4. Calls the `BuildingHider` component which hides any sub-mesh whose
   name matches `SM_Bld_*` or `SM_Prop_*` — that's how you "remove the
   buildings" in UE5 (you don't edit the file, you toggle component
   visibility/collision).
5. Updates `BP_GameMode.WorldOriginXY` / `WorldSizeXY` to match the
   imported bounds, so map-pin projection lines up.

## Tweaking what's hidden

Open the spawned `BP_PirateIslandWorld` actor in the level. Find the
**Hider** component:

| Property         | Effect                                                   |
|------------------|----------------------------------------------------------|
| `HidePrefixes`   | Default: `SM_Bld_`, `SM_Prop_`. Add more here.           |
| `KeepExceptions` | Names that stay visible even if their prefix matches.    |
| `bReveal`        | Flip to true + call `ApplyToOwner` to unhide everything. |

If you want a single ruined dock to remain after hiding all buildings,
add e.g. `SM_Bld_Dock_037` to `KeepExceptions`. The component
re-applies idempotently — call `ApplyToOwner` from BP whenever the
list changes.

## Scale & coordinates

Sketchfab's pirate islands are typically authored at metric scale.
UE5 imports glTF in **cm** (1 unit = 1 cm), so a 200-metre island
becomes 20,000 unit. The Python importer reads the actual computed
bounds and pushes them into `BP_GameMode`. If you re-scale the actor
afterwards, re-run the bounds-push step (it's idempotent).

## Surface counts (helpful for tuning)

The scene contains:

| Class            | Count |
|------------------|-------|
| Grass tufts       | 514  |
| Sugar cane        | 297  |
| Palm trees        | 129  |
| Beaches           | 120  |
| Huge rocks        | 113  |
| Standard rocks    | 92   |
| Large rocks       | 84   |
| **Docks**         | 84   |
| Mangrove trees    | 48   |
| Clouds            | 47   |
| Wall pieces       | ~140 (combined) |
| **Cuba rooms/roofs** | 65 |
| Other props       | ~150 |

After hiding all `SM_Bld_*` and `SM_Prop_*`, the world keeps grass,
trees, rocks, beaches, mangroves, clouds, ferns, vines, seaweed,
flowers — the "uncharted island" feel the design pillars ask for.

## Credit (CC-BY-4.0 requirement)

This work is based on **"chicken gun pirateislands reupload"**
(<https://sketchfab.com/3d-models/chicken-gun-pirateislands-reupload-893581df1abf4d35a0dd849b169722fc>)
by **amogusstrikesback2**
(<https://sketchfab.com/amogusstrikesback2>) licensed under
**CC-BY-4.0** (<http://creativecommons.org/licenses/by/4.0/>).

This credit is reproduced in the in-repo `license.txt` and must
appear in your game's credits screen and Steam page.
