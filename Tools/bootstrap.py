# Copyright (c) 2026 The Last Cartographer.
#
# ONE-COMMAND BOOTSTRAP. After cloning, dropping scene.bin in, and
# building C++:
#
#     1. Open CartographyGame.uproject
#     2. Window -> Output Log -> dropdown to Python
#     3. py "Tools/bootstrap.py"
#
# This composes everything: setup_project, brushes, IMC modifiers,
# widget stubs, level populate, gltf import. Idempotent. Designed so
# you can press Play immediately afterwards.

import os
import unreal

import setup_project as base_setup        # noqa: E402  (script-relative import)
import import_pirate_islands as importer   # noqa: E402
import build_widgets as widgets            # noqa: E402

EAS = unreal.EditorAssetLibrary
ALT = unreal.AssetToolsHelpers.get_asset_tools()
EUL = unreal.EditorLevelLibrary


def log(msg):  unreal.log("[bootstrap] " + msg)
def warn(msg): unreal.log_warning("[bootstrap] " + msg)


# ---------------------------------------------------------------------------
# 1. Brush materials
# ---------------------------------------------------------------------------

def make_brush_material(name, default_color, default_opacity, default_hardness):
    """Build a soft circular falloff brush material with parameters
    BrushColor, Opacity, Hardness. Domain: User Interface, Translucent."""
    full = "/Game/Materials/" + name
    if EAS.does_asset_exist(full):
        return EAS.load_asset(full)

    fac = unreal.MaterialFactoryNew()
    mat = ALT.create_asset(name, "/Game/Materials", unreal.Material, fac)
    if mat is None: return None

    MEL = unreal.MaterialEditingLibrary
    try:
        # Domain + blend
        mat.set_editor_property("material_domain", unreal.MaterialDomain.MD_UI)
        mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)

        tc = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -800, 0)

        sub = MEL.create_material_expression(mat, unreal.MaterialExpressionSubtract, -640, 0)
        sub.set_editor_property("const_a", unreal.LinearColor(0.5, 0.5, 0.5, 0))
        MEL.connect_material_expressions(tc, "", sub, "B")

        length = MEL.create_material_expression(mat, unreal.MaterialExpressionDistance, -480, 0)
        center = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant2Vector, -640, 100)
        center.set_editor_property("r", 0.0); center.set_editor_property("g", 0.0)
        MEL.connect_material_expressions(sub, "", length, "A")
        MEL.connect_material_expressions(center, "", length, "B")

        # Falloff: 1 - smoothstep(0.5*hardness, 0.5, dist)
        hardness = MEL.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -480, 200)
        hardness.set_editor_property("parameter_name", "Hardness")
        hardness.set_editor_property("default_value", default_hardness)

        half_h = MEL.create_material_expression(mat, unreal.MaterialExpressionMultiply, -320, 200)
        half_h.set_editor_property("const_b", 0.5)
        MEL.connect_material_expressions(hardness, "", half_h, "A")

        ss = MEL.create_material_expression(mat, unreal.MaterialExpressionSmoothStep, -160, 100)
        ss.set_editor_property("const_min", 0.0)
        ss.set_editor_property("const_max", 0.5)
        MEL.connect_material_expressions(half_h, "", ss, "Min")
        MEL.connect_material_expressions(length, "", ss, "Value")

        one_minus = MEL.create_material_expression(mat, unreal.MaterialExpressionOneMinus, 0, 100)
        MEL.connect_material_expressions(ss, "", one_minus, "")

        # Color
        color = MEL.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -160, -100)
        color.set_editor_property("parameter_name", "BrushColor")
        color.set_editor_property("default_value", default_color)

        opacity = MEL.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -160, 0)
        opacity.set_editor_property("parameter_name", "Opacity")
        opacity.set_editor_property("default_value", default_opacity)

        op_mul = MEL.create_material_expression(mat, unreal.MaterialExpressionMultiply, 160, 100)
        MEL.connect_material_expressions(one_minus, "", op_mul, "A")
        MEL.connect_material_expressions(opacity, "", op_mul, "B")

        # Outputs
        MEL.connect_material_property(color, "RGB", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
        MEL.connect_material_property(op_mul, "", unreal.MaterialProperty.MP_OPACITY)

        MEL.recompile_material(mat)
        EAS.save_asset(full)
        log("Material " + full + " built")
    except Exception as e:
        warn("Material build failed for {}: {}".format(name, e))
    return mat


def setup_brush_materials():
    log("Brush materials")
    pencil = make_brush_material("M_PencilBrush", unreal.LinearColor(0.10, 0.08, 0.07, 1.0), 0.55, 0.4)
    eraser = make_brush_material("M_EraserBrush", unreal.LinearColor(0.92, 0.86, 0.74, 1.0), 1.0,  0.5)
    ink    = make_brush_material("M_InkBrush",    unreal.LinearColor(0.04, 0.03, 0.02, 1.0), 1.0,  0.95)

    # Wire onto BP_Cartographer's FieldMap component defaults.
    bp = EAS.load_asset("/Game/Blueprints/BP_Cartographer")
    if bp:
        try:
            cdo = unreal.get_default_object(bp.generated_class())
            field_map = cdo.get_editor_property("FieldMap") if cdo else None
            if field_map:
                if pencil: field_map.set_editor_property("PencilBrushMaterial", pencil)
                if eraser: field_map.set_editor_property("EraserBrushMaterial", eraser)
                if ink:    field_map.set_editor_property("InkBrushMaterial", ink)
            EAS.save_asset(bp.get_path_name())
        except Exception as e:
            warn("Brush wiring failed: " + str(e))


# ---------------------------------------------------------------------------
# 2. IMC modifiers (WASD swizzle + negate)
# ---------------------------------------------------------------------------

def setup_imc_modifiers():
    log("IMC WASD modifiers")
    imc = EAS.load_asset("/Game/Input/IMC_Default")
    move = EAS.load_asset("/Game/Input/IA_Move")
    if not imc or not move:
        warn("IMC_Default or IA_Move missing — run setup_project first")
        return

    try:
        EI = unreal.EnhancedInputLibrary

        existing = imc.get_editor_property("mappings") or []

        def has_mapping(action, key_name):
            for m in existing:
                if m.action == action and str(m.key.key_name) == key_name:
                    return True
            return False

        def add(action, key_name, modifiers):
            if has_mapping(action, key_name):
                return
            mapping = unreal.EnhancedActionKeyMapping()
            mapping.action = action
            k = unreal.InputCoreLibrary.get_key_for_name(unreal.Name(key_name)) if hasattr(unreal, 'InputCoreLibrary') else None
            if k is None:
                k = unreal.Key(key_name)
            mapping.key = k
            mapping.modifiers = modifiers
            existing.append(mapping)

        swizzle = unreal.InputModifierSwizzleAxis()
        swizzle.set_editor_property("order", unreal.InputAxisSwizzle.YXZ)
        negate_y = unreal.InputModifierNegate()  # default negates X by default; we keep simple for W/S
        negate   = unreal.InputModifierNegate()

        # W -> +Y, S -> -Y, A -> -X, D -> +X. Axis2D layout: (X=horizontal, Y=vertical).
        # W needs swizzle (so the bool 1 ends up on Y, not X). S needs swizzle + negate.
        add(move, "W", [swizzle])
        add(move, "S", [swizzle, negate])
        add(move, "A", [negate_y])
        add(move, "D", [])

        imc.set_editor_property("mappings", existing)
        EAS.save_asset(imc.get_path_name())
        log("IMC modifiers added.")
    except Exception as e:
        warn("IMC modifier wiring failed: " + str(e))


# ---------------------------------------------------------------------------
# 3. UMG widget stubs (so the C++ NativeTicks have something to find)
# ---------------------------------------------------------------------------

def make_widget_bp(name, parent_class_name=None):
    full = "/Game/UI/" + name
    if EAS.does_asset_exist(full):
        return EAS.load_asset(full)
    fac = unreal.WidgetBlueprintFactory()
    if parent_class_name:
        parent = unreal.load_class(None, "/Script/CartographyGame." + parent_class_name)
        if parent: fac.set_editor_property("parent_class", parent)
    bp = ALT.create_asset(name, "/Game/UI", unreal.WidgetBlueprint, fac)
    if bp is None: return None
    EAS.save_asset(full)
    log("Widget " + full + " created")
    return bp


def add_named_child(widget_bp, child_class, child_name):
    """Add a single child to the widget tree with the BindWidget name
    the C++ base expects. Skips if already present."""
    try:
        tree = widget_bp.get_editor_property("widget_tree")
        # Find existing
        all_widgets = unreal.UMGEditorLibrary.list_child_widgets(tree.root_widget) if tree.root_widget else []
        for w in all_widgets:
            if w.get_name() == child_name:
                return w
    except Exception:
        pass
    try:
        # Use SubobjectDataSubsystem-equivalent path: WidgetBlueprintLibrary.create_widget
        widget = unreal.UMGEditorLibrary.add_widget_to_canvas(widget_bp, child_class, child_name) \
            if hasattr(unreal, 'UMGEditorLibrary') else None
        return widget
    except Exception as e:
        warn("Child add failed for {}: {}".format(child_name, e))
        return None


def setup_widgets():
    log("Widget Blueprints")
    # Stubs that the C++ NativeTick code references. Minimal but valid;
    # designer fleshes layout per Docs/13-UI.md.
    widgets = [
        ("WBP_HUD",            "HUDWidget"),
        ("WBP_Compass",        "CompassWidget"),
        ("WBP_DiscoveryToast", "DiscoveryToastWidget"),
        ("WBP_FieldMap",       None),
        ("WBP_Notebook",       None),
        ("WBP_Inking",         None),
        ("WBP_Pause",          None),
    ]
    created = {}
    for name, parent in widgets:
        bp = make_widget_bp(name, parent)
        if bp: created[name] = bp

    # Wire to BP_PlayerController
    bp_pc = EAS.load_asset("/Game/Blueprints/BP_PlayerController")
    if bp_pc:
        try:
            cdo = unreal.get_default_object(bp_pc.generated_class())
            for prop, wbp_name in [
                ("HUDClass",       "WBP_HUD"),
                ("FieldMapClass",  "WBP_FieldMap"),
                ("NotebookClass",  "WBP_Notebook"),
                ("InkingClass",    "WBP_Inking"),
                ("PauseMenuClass", "WBP_Pause"),
            ]:
                bp = created.get(wbp_name)
                if bp and cdo:
                    cdo.set_editor_property(prop, bp.generated_class())
            EAS.save_asset(bp_pc.get_path_name())
        except Exception as e:
            warn("PC widget wiring failed: " + str(e))


# ---------------------------------------------------------------------------
# 4. Populate L_Island with sky/sun/floor/player start
# ---------------------------------------------------------------------------

def populate_island():
    log("Populating L_Island")
    full = "/Game/Maps/L_Island"
    if not EAS.does_asset_exist(full):
        warn("L_Island missing — base setup didn't create it.")
        return
    try:
        ls = unreal.LevelEditorSubsystem()
        ls.load_level(full)
    except Exception:
        EUL.load_level(full)

    # Existing actor names to avoid duplicating.
    existing = {a.get_actor_label() for a in EUL.get_all_level_actors()}

    def spawn(cls, label, location, rotation=unreal.Rotator(0,0,0)):
        if label in existing: return None
        actor = EUL.spawn_actor_from_class(cls, location, rotation)
        if actor is not None:
            actor.set_actor_label(label)
            log("  + " + label)
        return actor

    # Sun (DirectionalLight tagged "SunLight")
    sun = spawn(unreal.DirectionalLight, "SunLight",
                unreal.Vector(0, 0, 5000), unreal.Rotator(-45, 0, 0))
    if sun:
        try:
            sun.set_mobility(unreal.ComponentMobility.MOVABLE)
            sun.tags = [unreal.Name("SunLight")]
        except Exception:
            pass

    # Sky atmosphere
    spawn(unreal.SkyAtmosphere, "SkyAtmosphere", unreal.Vector(0, 0, 0))

    # Sky light
    sky = spawn(unreal.SkyLight, "SkyLight", unreal.Vector(0, 0, 1500))
    if sky:
        try: sky.set_mobility(unreal.ComponentMobility.MOVABLE)
        except Exception: pass

    # Exponential height fog
    spawn(unreal.ExponentialHeightFog, "HeightFog", unreal.Vector(0, 0, 0))

    # Volumetric cloud
    spawn(unreal.VolumetricCloud, "VolumetricCloud", unreal.Vector(0, 0, 0))

    # Player start at a reasonable spot above origin
    spawn(unreal.PlayerStart, "PlayerStart", unreal.Vector(0, 0, 600))

    # A simple water plane at z=0 (visible until you tweak/replace).
    # Engine's basic StaticMeshActor with the engine plane mesh.
    plane_mesh = EAS.load_asset("/Engine/BasicShapes/Plane.Plane")
    water_actor = spawn(unreal.StaticMeshActor, "WaterPlane",
                        unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    if water_actor and plane_mesh:
        try:
            water_actor.set_actor_scale3d(unreal.Vector(2000, 2000, 1))  # huge
            comp = water_actor.get_component_by_class(unreal.StaticMeshComponent)
            if comp: comp.set_static_mesh(plane_mesh)
        except Exception as e:
            warn("Water plane setup failed: " + str(e))

    # Drop the BP_WorldHider so buildings hide on Play.
    if "WorldHider" not in existing:
        bp_h = EAS.load_asset("/Game/Blueprints/BP_WorldHider")
        if bp_h:
            spawn(bp_h.generated_class(), "WorldHider", unreal.Vector(0, 0, 100))

    try:
        EUL.save_current_level()
    except Exception as e:
        warn("save level failed: " + str(e))


# ---------------------------------------------------------------------------
# 5. Sample gameplay content in L_Island
# ---------------------------------------------------------------------------

def populate_sample_content():
    log("Sample content (inking desk, landmarks, fast-travel)")

    def existing_labels():
        return {a.get_actor_label() for a in EUL.get_all_level_actors()}

    def spawn_bp(bp_path, label, location):
        if label in existing_labels(): return None
        bp = EAS.load_asset(bp_path)
        if bp is None: return None
        actor = EUL.spawn_actor_from_class(bp.generated_class(), location)
        if actor is not None:
            actor.set_actor_label(label)
            log("  + " + label)
        return actor

    # Inking desk near origin (for the "go home and ink" loop).
    spawn_bp("/Game/Blueprints/BP_InkingDesk", "Cabin_InkingDesk",
             unreal.Vector(800, 0, 100))

    # GroundTruthCapture high above the world centre.
    gt = spawn_bp("/Game/Blueprints/BP_GroundTruthCapture",
                  "GroundTruth_Capture", unreal.Vector(0, 0, 30000))
    if gt:
        try:
            gt.set_actor_rotation(unreal.Rotator(-90, 0, 0), False)
        except Exception:
            pass

    # A couple of hidden landmarks for the "reveal on accuracy" demo.
    for label, loc in [
        ("Hidden_Beacon",  unreal.Vector( 6000,  4000, 1500)),
        ("Hidden_Shrine",  unreal.Vector(-5500, -3500, 1200)),
        ("Hidden_Spring",  unreal.Vector( 2000, -6500,  900)),
    ]:
        spawn_bp("/Game/Landmarks/BP_HiddenLandmark", label, loc)

    # Fast-travel stones — one at cabin, two at far points.
    for label, loc in [
        ("FastTravel_Cabin", unreal.Vector(   600, 0,  100)),
        ("FastTravel_North", unreal.Vector(-15000, 0, 1500)),
        ("FastTravel_East",  unreal.Vector(     0, 15000, 1500)),
    ]:
        spawn_bp("/Game/Blueprints/BP_FastTravelStone", label, loc)

    try: EUL.save_current_level()
    except Exception: pass


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def run():
    log("==========================================")
    log("    The Last Cartographer — bootstrap")
    log("==========================================")
    base_setup.run()
    setup_brush_materials()
    setup_imc_modifiers()
    setup_widgets()
    widgets.run()              # lay out actual UMG content
    populate_island()
    populate_sample_content()  # one inking desk + a few landmarks

    # Optional: glTF import if scene.bin is in place.
    bin_path = os.path.join(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()),
                            "Content", "ImportSource", "PirateIslands", "scene.bin")
    if os.path.isfile(bin_path):
        importer.run()
    else:
        warn("scene.bin not found; skipping glTF import. Drop it into "
             "Content/ImportSource/PirateIslands/ and re-run, or run "
             "import_pirate_islands.py directly.")

    log("==========================================")
    log("  Done. Press Play to walk the islands.")
    log("==========================================")


if __name__ == "__main__":
    run()
