# System 18 — Procedural Island

`AProceduralIsland` builds a heightmap mesh from layered noise + a
radial coastline falloff. The result: a single actor you can drop into
an empty level for instant walkable terrain, no sculpting required.

## Place one

1. Open `Content/Maps/L_Island`.
2. Drag `BP_ProceduralIsland` (created by `setup_project.py`) into the
   level at world origin.
3. Defaults that work out of the box:
   - `Resolution = 192` (37k triangles — playable on a laptop)
   - `Size = (100000, 100000)` cm (1 km square)
   - `MaxAltitude = 9000` cm
   - `Seed = 1337` — change for a different island shape
   - `CoastFalloffPower = 2.5` — higher = sharper coastline, lower = flatter
4. The actor calls `Rebuild` in `OnConstruction`. Drag any property in
   the Details panel and the mesh refreshes immediately.

## Land material (optional)

Without `LandMaterial` you'll see plain default material. Make a simple
landscape-style material that uses `Vertex Color.R` as a height tint
into your three terrain albedos (sand → grass → rock) and assign it.

## Exposed query

`SampleHeightAt(WorldXY)` returns the mesh height at any horizontal
location. Useful for placing landmarks/wildlife exactly on the surface.

## Performance

192² resolution = ~74k triangles. Increasing past 256 is fine but
`bUseAsyncCooking = true` is set so the editor doesn't freeze on
parameter changes.
