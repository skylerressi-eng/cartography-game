# System 12 — Fast Travel

`UFastTravelSubsystem` registers `AFastTravelMarker` actors as the
player overlaps their activation sphere. A marker becomes
**discovered** on first overlap and **available** when the cell
containing it is `AccuratelyMapped`.

## Setup

1. `Content/Blueprints/`, BP child of `FastTravelMarker` →
   `BP_FastTravelStone`. Add a stylised cairn mesh.
2. Place markers at: cabin, north cape, east woods entrance, south
   cove, summit. Set `MarkerId` to a unique name; `RequiresAccurateMapping`
   stays true except for the cabin itself.
3. On the field-map widget, add a "Fast Travel" panel:
   - On Construct, call `Subsystem → Get Available()` for the marker
     list.
   - For each result, show a button → `Subsystem → TravelTo(Player,
     MarkerId)`.

## Edge cases

- A marker whose cell becomes inaccurate again **stops being
  available** until re-inked. Player keeps the discovered state, but
  can't travel through fog.
- The cabin is hand-flagged with `bRequiresAccurateMapping = false`
  so the player can always come home.
