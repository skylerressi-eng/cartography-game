# System 2 — Field Map (portable drawing)

The C++ does the math. You build the canvas asset, brush materials, and
the UI widget.

## 1. Render targets

In `Content/RenderTargets/`, right-click → **Materials & Textures →
Render Target**:

- `RT_FieldMap`  — open it, set Size X/Y = `2048`, format = `RTF RGBA8`.
- `RT_MasterMap` — same.

`UFieldMapComponent` will create transient ones if these are unassigned,
but using assets lets the editor preview them.

## 2. Brush materials

For each brush, create a material in `Content/Materials/`:

### `M_PencilBrush`

- Material Domain: **User Interface**
- Blend Mode: **Translucent**
- Add scalar parameters: `Hardness` (default 0.4), `Opacity` (default 0.55).
- Add vector parameter: `BrushColor` (default `(0.10, 0.08, 0.07, 1)`).
- Soft circular falloff:
  - `TexCoord[0]` → subtract `(0.5, 0.5)` → `Length` → call this `r`.
  - `1 - smoothstep(0.5*Hardness, 0.5, r)` → call this `Mask`.
  - `BrushColor.RGB` → connect to **Final Color**.
  - `Mask * Opacity` → connect to **Opacity**.

### `M_EraserBrush`

- Same shape as the pencil but `BrushColor` defaults to the parchment
  colour `(0.92, 0.86, 0.74, 1)`, `Opacity` defaults to `1`,
  `Hardness` defaults to `0.5`.

### `M_InkBrush`

- Same falloff but **opaque-looking** — `Opacity` = 1, `Hardness` = 0.95,
  `BrushColor` = `(0.04, 0.03, 0.02, 1)`.

In `BP_Cartographer`'s `FieldMap` component (under Components on the
left), assign:

- `Field Map RT` → `RT_FieldMap`
- `Master Map RT` → `RT_MasterMap`
- `Pencil Brush Material` → `M_PencilBrush`
- `Eraser Brush Material` → `M_EraserBrush`
- `Ink Brush Material` → `M_InkBrush`
- `Parchment Color` → `(0.92, 0.86, 0.74, 1)`

## 3. Field Map widget

1. `Content/UI/`, right-click → **User Interface → Widget Blueprint** →
   `WBP_FieldMap`.
2. Root Canvas Panel → drop a **Border** filling the screen, parchment
   colour. Inside the Border, drop an **Image**, anchored full-size,
   set **Brush → Texture** = `RT_FieldMap`. Name it `Img_Map`.
3. Add an **Overlay** child — this is where we'll place an invisible
   **Image** that captures mouse events. Name it `MouseSurface`. Set
   `Visibility = Visible`, `Brush → Tint` = transparent.
4. Add a **Horizontal Box** at the bottom for tool buttons: `Btn_Pencil`,
   `Btn_Eraser`. Wire them to a `CurrentBrush` enum variable (Pencil/
   Eraser) on the widget.

### Widget graph (event flow)

- **Construct** → `Get Owning Player Pawn` → cast to `BP_Cartographer` →
  promote to variable `Player`.
- Add an `OnMouseMove` override on `MouseSurface` that:
  1. Gets the local mouse position via
     `Geometry → Absolute To Local`.
  2. Divides by `Geometry → Get Local Size` → that's `CurrUV`
     (clamped 0..1).
  3. If `IsDrawing` boolean is true:
     - Calls `Player → FieldMap → Draw Stroke`(`PrevUV`, `CurrUV`,
       `FieldMap`, `CurrentBrush`).
     - Stores `CurrUV` as `PrevUV`.
- `OnMouseButtonDown` (Left): set `IsDrawing = true`, store `PrevUV`,
  call `Player → FieldMap → Draw At`(`PrevUV`, `FieldMap`, `CurrentBrush`).
  Return `Handled`.
- `OnMouseButtonUp` (Left): set `IsDrawing = false`. Return `Handled`.

## 4. Toggle the widget on `M`

In `BP_Cartographer`, override the BlueprintImplementableEvent
`OnToggleFieldMap`:

- If a `FieldMapWidget` reference is invalid:
  - `Create Widget`(`WBP_FieldMap`) → store in `FieldMapWidget`.
- If `Is Field Map Open` returns `true` (the C++ already toggled the bool):
  - `FieldMapWidget → Add To Viewport`
  - `Get Player Controller(0) → Set Show Mouse Cursor (true)`
  - `Get Player Controller(0) → Set Input Mode UI Only` with
    `Widget To Focus = FieldMapWidget`
- Else:
  - `FieldMapWidget → Remove From Parent` (don't destroy — keep the RT)
  - `Set Show Mouse Cursor (false)`
  - `Set Input Mode Game Only`

The render targets persist across open/close because they live on the
component, not the widget.

## 5. Test

Press **Play**, press **M**. The parchment fills the screen. Hold LMB
and drag — you draw pencil. Switch to eraser. Press **M** again to
return to walking; reopen and your strokes are still there.
