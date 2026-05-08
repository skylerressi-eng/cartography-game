# System 3 — Field Notebook & Map Pins

## 1. Pin Blueprint

1. `Content/Pins/`, right-click → **Blueprint Class** → `MapPin` parent
   → `BP_MapPin_Default`.
2. Open it. Replace the `Mesh` static mesh with an engine cone or a
   simple flag mesh, scale to ~25 cm tall.
3. Compile, save.

## 2. Wire `BP_Cartographer` to spawn pins

In `BP_Cartographer`'s `Notebook` component (Components panel):

- Set `Pin Class` = `BP_MapPin_Default`.
- Set `World Origin XY` and `World Size XY` to match your island bounds
  (same values you'll set on the GroundTruthCapture and the
  WorldResponseSubsystem). For a 1 km² island centred on origin: Origin
  = `(-50000, -50000)`, Size = `(100000, 100000)` (Unreal units = cm).

## 3. Notebook widget

1. `Content/UI/`, **Widget Blueprint** → `WBP_Notebook`.
2. Layout: a Vertical Box of existing entries (each entry = a smaller
   widget `WBP_NotebookEntry` with Title text, Body text, "Go to Pin"
   button), plus an "Add Note Here" button at the bottom.
3. The "Add Note Here" button calls `Player → Notebook → Pin At Player
   Location` with a Title and Body filled from text inputs.
4. Bind to the `OnNotebookChanged` delegate to refresh the list.

## 4. Toggle on `N`

In `BP_Cartographer`, implement `OnToggleNotebook` mirroring
`OnToggleFieldMap`. Keep the widget instance around to preserve scroll/
edit state.

## 5. Show pins on the field map

Inside `WBP_FieldMap` (System 2 widget):

- Add a Canvas Panel **above** `Img_Map`, name it `PinLayer`, fill the
  same area.
- On `Construct` (and on every `OnNotebookChanged` event):
  1. Clear `PinLayer`.
  2. For each entry in `Player → Notebook → Entries`:
     - `Player → Notebook → World To Map UV (Entry.WorldLocation)` → UV.
     - Multiply by `PinLayer → Get Local Size` → `PinPx`.
     - Create a small `WBP_PinIcon` widget (an Image with the marker
       colour) and add to `PinLayer` at `PinPx` with anchor `(0.5, 0.5)`.

## 6. Test

Walk to a spot in the world, press `N`, type a title, click "Add Note
Here". Press `M` — the pin should appear at the correct location on
the parchment.
