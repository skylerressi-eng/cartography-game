# System 4 — Home Studio (Inking)

## 1. Cabin geometry

For a placeholder, use **Modeling Mode** (top toolbar, modelling menu)
to box-out a 5 m × 5 m room with a doorway:

- Floor: `Box Brush` 500×500×10 cm.
- Walls: four walls 500×10×280 cm with one wall having a 100×210 cm hole.
- Drop a desk (any cube scaled to ~150×60×80 cm) at the centre of one
  wall, a chair, and a candle.

You'll replace these meshes with stylised art later.

## 2. `BP_InkingDesk`

1. `Content/Blueprints/`, right-click → **Blueprint Class** →
   parent class `InkingDesk` → `BP_InkingDesk`.
2. Open it. Set `DeskMesh` static mesh to your desk cube.
3. The default `InteractVolume` extends 120 cm forward and 100 cm up;
   tweak as needed.

Place one `BP_InkingDesk` in your cabin.

## 3. Hook up the interact key

In `BP_Cartographer`, override `OnInteractPressed`:

1. `Get Overlapping Actors`(self) filtered by `BP_InkingDesk` →
   if any → `Try Begin Inking(self)`.

In `BP_InkingDesk`'s `OnInkingBegan` event (right-click in Event Graph
→ search `Assign On Inking Began` after dragging from the desk
reference, or set up a custom event in the desk that fires on
`OnInkingBegan` directly):

1. Cast Pawn to `BP_Cartographer`.
2. Create `WBP_Inking` (next step) → store reference → Add to Viewport.
3. `Set Input Mode UI Only`, show mouse cursor.

## 4. `WBP_Inking` widget

Two **Image** widgets side-by-side in a Horizontal Box:

- Left image bound to `RT_FieldMap` (read-only).
- Right image bound to `RT_MasterMap` (this is the canvas the player
  draws on).
- Below them a **Progress Bar** bound to
  `Player.FieldMap.CurrentInk / Player.FieldMap.MaxInk`.
- A "Done" button that calls `Player → Get Inking Desk → End Inking
  (Player)` and removes this widget from parent.

The right image's mouse handlers mirror System 2's, but call
`DrawStroke` with `MasterMap` and `Ink` brush mode.

## 5. Ink resource

The C++ already debits ink from `CurrentInk` on each ink stroke stamp.

To replenish: in your "rest in cabin" Blueprint (a separate trigger
on the bed), call `BP_InkingDesk → Refill Ink (Player)` — this restores
to max. You can also wire `OnInkingEnded` to give a small refill, but
the design says only sleeping fully refills.

## 6. After inking ends

When the player finishes inking and exits, fire the comparison
(`Docs/05-Comparison.md`) so world response updates before they walk
out the door.
