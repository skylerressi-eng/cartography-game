# Copyright (c) 2026 The Last Cartographer.
#
# UE5 Editor Python — auto-create binary assets the C++ scaffold expects.
#
# Run inside the editor:
#   1. Open CartographyGame.uproject (let it compile).
#   2. Window -> Output Log.
#   3. Switch the bottom dropdown from "Cmd" to "Python".
#   4. Type:    py "Tools/setup_project.py"
#
# Idempotent — re-running won't duplicate anything.

import unreal


EAS = unreal.EditorAssetLibrary
ALT = unreal.AssetToolsHelpers.get_asset_tools()


def log(msg): unreal.log("[Cartography setup] " + msg)
def warn(msg): unreal.log_warning("[Cartography setup] " + msg)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def factory(*candidate_names):
    """Return an instance of the first available factory class on `unreal`.
    UE5.x renames factory classes between minor versions — try a few."""
    for n in candidate_names:
        cls = getattr(unreal, n, None)
        if cls is not None:
            try:
                return cls()
            except Exception as e:
                warn("Factory {} construction failed: {}".format(n, e))
    return None


def ensure_folder(path):
    if not EAS.does_directory_exist(path):
        EAS.make_directory(path)


def create_asset_safe(name, path, asset_class, fac):
    full = path + "/" + name
    if EAS.does_asset_exist(full):
        return EAS.load_asset(full)
    if fac is None:
        warn("No factory for " + name + " — skipping")
        return None
    try:
        a = ALT.create_asset(name, path, asset_class, fac)
        if a is not None:
            EAS.save_asset(full)
            log("Created " + full)
        return a
    except Exception as e:
        warn("Create failed for {}: {}".format(full, e))
        return None


def find_class(name):
    return unreal.load_class(None, "/Script/CartographyGame." + name)


# ---------------------------------------------------------------------------
# Folders
# ---------------------------------------------------------------------------

FOLDERS = [
    "/Game/Maps", "/Game/Blueprints", "/Game/UI", "/Game/Materials",
    "/Game/RenderTargets", "/Game/Input", "/Game/Pins", "/Game/Landmarks",
    "/Game/Audio/SoundClasses", "/Game/Audio/Music", "/Game/Audio/Ambient",
    "/Game/Data", "/Game/MPC",
]


def setup_folders():
    log("Folders")
    for f in FOLDERS:
        ensure_folder(f)


# ---------------------------------------------------------------------------
# Render targets
# ---------------------------------------------------------------------------

def setup_render_targets():
    log("Render targets")
    fac = factory("TextureRenderTarget2DFactoryNew", "CanvasRenderTarget2DFactoryNew")
    for name in ("RT_FieldMap", "RT_MasterMap", "RT_GroundTruth"):
        rt = create_asset_safe(name, "/Game/RenderTargets",
                               unreal.TextureRenderTarget2D, fac)
        if rt:
            try:
                rt.set_editor_property("size_x", 2048)
                rt.set_editor_property("size_y", 2048)
                rt.set_editor_property("render_target_format",
                                       unreal.TextureRenderTargetFormat.RTF_RGBA8)
                EAS.save_asset(rt.get_path_name())
            except Exception as e:
                warn("RT property set failed: " + str(e))


# ---------------------------------------------------------------------------
# Sound classes
# ---------------------------------------------------------------------------

def setup_sound_classes():
    log("Sound classes")
    fac = factory("SoundClassFactory")
    for n in ("SC_Master", "SC_Music", "SC_Ambient", "SC_SFX"):
        create_asset_safe(n, "/Game/Audio/SoundClasses", unreal.SoundClass, fac)


# ---------------------------------------------------------------------------
# MPC
# ---------------------------------------------------------------------------

def setup_mpc():
    log("Material parameter collection")
    fac = factory("MaterialParameterCollectionFactoryNew")
    mpc = create_asset_safe("MPC_World", "/Game/MPC",
                            unreal.MaterialParameterCollection, fac)
    if mpc is None:
        return
    try:
        scalars = []
        for n, v in [("TimeOfDay", 12.0), ("WeatherIntensity", 0.0),
                     ("Fog", 0.2), ("AccuracyGlitch", 0.0)]:
            p = unreal.CollectionScalarParameter()
            p.set_editor_property("parameter_name", n)
            p.set_editor_property("default_value", v)
            scalars.append(p)
        vectors = []
        for n, v in [("WindDirection", unreal.LinearColor(1, 0, 0, 0)),
                     ("BiomeTint",     unreal.LinearColor(1, 1, 1, 1))]:
            p = unreal.CollectionVectorParameter()
            p.set_editor_property("parameter_name", n)
            p.set_editor_property("default_value", v)
            vectors.append(p)
        mpc.set_editor_property("scalar_parameters", scalars)
        mpc.set_editor_property("vector_parameters", vectors)
        EAS.save_asset(mpc.get_path_name())
    except Exception as e:
        warn("MPC parameter setup failed: " + str(e))


# ---------------------------------------------------------------------------
# Input — actions + mapping context
# ---------------------------------------------------------------------------

INPUT_ACTIONS = [
    ("IA_Move",            "AXIS2D"),
    ("IA_Look",            "AXIS2D"),
    ("IA_Jump",            "BOOLEAN"),
    ("IA_ToggleFieldMap",  "BOOLEAN"),
    ("IA_ToggleNotebook",  "BOOLEAN"),
    ("IA_Interact",        "BOOLEAN"),
    ("IA_Draw",            "BOOLEAN"),
    ("IA_Pause",           "BOOLEAN"),
    ("IA_QuickSave",       "BOOLEAN"),
]


def setup_input():
    log("Input actions + mapping context")
    fac = factory("InputActionFactory")
    actions = {}
    for name, kind in INPUT_ACTIONS:
        ia = create_asset_safe(name, "/Game/Input", unreal.InputAction, fac)
        if ia is not None:
            try:
                vt = getattr(unreal.InputActionValueType, kind, None)
                if vt is not None: ia.set_editor_property("value_type", vt)
                EAS.save_asset(ia.get_path_name())
            except Exception:
                pass
            actions[name] = ia
    fac = factory("InputMappingContextFactory")
    create_asset_safe("IMC_Default", "/Game/Input",
                      unreal.InputMappingContext, fac)
    log("IMC_Default created empty — open it in editor and add WASD/mouse "
        "mappings per Docs/01-Player.md (Python's IMC API in 5.x is "
        "incomplete for axis modifiers).")


# ---------------------------------------------------------------------------
# Blueprints
# ---------------------------------------------------------------------------

BP_DEFS = [
    ("BP_Cartographer",       "CartographerCharacter",   "/Game/Blueprints"),
    ("BP_GameMode",           "CartographyGameMode",     "/Game/Blueprints"),
    ("BP_PlayerController",   "CartographyPlayerController", "/Game/Blueprints"),
    ("BP_InkingDesk",         "InkingDesk",              "/Game/Blueprints"),
    ("BP_GroundTruthCapture", "GroundTruthCapture",      "/Game/Blueprints"),
    ("BP_FastTravelStone",    "FastTravelMarker",        "/Game/Blueprints"),
    ("BP_HiddenLandmark",     "HiddenLandmark",          "/Game/Landmarks"),
    ("BP_MapPin",             "MapPin",                  "/Game/Pins"),
    ("BP_Wildlife",           "Wildlife",                "/Game/Blueprints"),
    ("BP_ProceduralIsland",   "ProceduralIsland",        "/Game/Blueprints"),
    ("BP_PirateIslandWorld",  "PirateIslandWorld",       "/Game/Blueprints"),
    ("BP_WorldHider",         "WorldHider",              "/Game/Blueprints"),
]


def setup_blueprints():
    log("Blueprints")
    fac = factory("BlueprintFactory")
    for name, parent_name, path in BP_DEFS:
        full = path + "/" + name
        if EAS.does_asset_exist(full): continue
        parent = find_class(parent_name)
        if parent is None:
            warn("Skip " + name + " — C++ class not found (recompile?)")
            continue
        try:
            fac.set_editor_property("parent_class", parent)
            bp = ALT.create_asset(name, path, unreal.Blueprint, fac)
            if bp:
                EAS.save_asset(full)
                log("Created BP " + full)
        except Exception as e:
            warn("BP create failed for {}: {}".format(name, e))


# ---------------------------------------------------------------------------
# Data tables
# ---------------------------------------------------------------------------

DATA_TABLES = [
    ("DT_Regions",      "FRegionRow"),
    ("DT_Biomes",       "FBiomeRow"),
    ("DT_Tools",        "FToolRow"),
    ("DT_Landmarks",    "FLandmarkRow"),
    ("DT_Lore",         "FLoreRow"),
    ("DT_Achievements", "FAchievementRow"),
]


def setup_datatables():
    log("DataTables")
    project_dir = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
    for name, row in DATA_TABLES:
        full = "/Game/Data/" + name
        if EAS.does_asset_exist(full): continue
        csv = project_dir + "Content/Data/" + name + ".csv"
        if not unreal.Paths.file_exists(csv):
            warn("Missing CSV: " + csv); continue
        try:
            task = unreal.AssetImportTask()
            task.filename = csv
            task.destination_path = "/Game/Data"
            task.destination_name = name
            task.replace_existing = True
            task.automated = True
            task.save = True
            settings = unreal.CSVImportSettings()
            settings.import_type = unreal.CSVImportType.ECSV_DATA_TABLE
            struct = unreal.load_object(None, "/Script/CartographyGame." + row)
            if struct: settings.import_row_struct = struct
            task.options = settings
            ALT.import_asset_tasks([task])
            log("Imported " + full)
        except Exception as e:
            warn("Import failed for {}: {}".format(name, e))


# ---------------------------------------------------------------------------
# Wire defaults on the new BPs
# ---------------------------------------------------------------------------

def wire_defaults():
    log("Wiring BP defaults")
    rt_field   = EAS.load_asset("/Game/RenderTargets/RT_FieldMap")
    rt_master  = EAS.load_asset("/Game/RenderTargets/RT_MasterMap")
    rt_truth   = EAS.load_asset("/Game/RenderTargets/RT_GroundTruth")
    imc        = EAS.load_asset("/Game/Input/IMC_Default")

    bp = EAS.load_asset("/Game/Blueprints/BP_Cartographer")
    if bp:
        try:
            cdo = unreal.get_default_object(bp.generated_class())
            if imc and cdo: cdo.set_editor_property("default_mapping_context", imc)
            for prop, ia_name in [
                ("MoveAction","IA_Move"), ("LookAction","IA_Look"),
                ("JumpAction","IA_Jump"),
                ("ToggleFieldMapAction","IA_ToggleFieldMap"),
                ("ToggleNotebookAction","IA_ToggleNotebook"),
                ("InteractAction","IA_Interact"),
                ("DrawAction","IA_Draw")]:
                ia = EAS.load_asset("/Game/Input/" + ia_name)
                if ia and cdo: cdo.set_editor_property(prop, ia)
            field_map = cdo.get_editor_property("FieldMap") if cdo else None
            if field_map:
                if rt_field:  field_map.set_editor_property("FieldMapRT", rt_field)
                if rt_master: field_map.set_editor_property("MasterMapRT", rt_master)
            EAS.save_asset(bp.get_path_name())
        except Exception as e:
            warn("BP_Cartographer wiring failed: " + str(e))

    bp_gm = EAS.load_asset("/Game/Blueprints/BP_GameMode")
    if bp_gm:
        try:
            cdo = unreal.get_default_object(bp_gm.generated_class())
            for table_name, prop in [
                ("DT_Regions","RegionsTable"), ("DT_Biomes","BiomesTable"),
                ("DT_Tools","ToolsTable"), ("DT_Landmarks","LandmarksTable"),
                ("DT_Lore","LoreTable"), ("DT_Achievements","AchievementsTable")]:
                t = EAS.load_asset("/Game/Data/" + table_name)
                if t and cdo: cdo.set_editor_property(prop, t)
            pawn = EAS.load_asset("/Game/Blueprints/BP_Cartographer")
            pc   = EAS.load_asset("/Game/Blueprints/BP_PlayerController")
            if pawn and cdo: cdo.set_editor_property("default_pawn_class", pawn.generated_class())
            if pc   and cdo: cdo.set_editor_property("player_controller_class", pc.generated_class())
            EAS.save_asset(bp_gm.get_path_name())
        except Exception as e:
            warn("BP_GameMode wiring failed: " + str(e))

    bp_gt = EAS.load_asset("/Game/Blueprints/BP_GroundTruthCapture")
    if bp_gt and rt_truth:
        try:
            cdo = unreal.get_default_object(bp_gt.generated_class())
            if cdo: cdo.set_editor_property("GroundTruthRT", rt_truth)
            EAS.save_asset(bp_gt.get_path_name())
        except Exception as e:
            warn("BP_GroundTruthCapture wiring failed: " + str(e))


# ---------------------------------------------------------------------------
# Level creation: empty L_Island
# ---------------------------------------------------------------------------

def setup_level():
    log("Level L_Island")
    full = "/Game/Maps/L_Island"
    if EAS.does_asset_exist(full):
        log("L_Island exists, skipping")
        return
    try:
        ls = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        ls.new_level(full)
        log("Created /Game/Maps/L_Island")
    except Exception as e:
        # Fallback for older API
        try:
            unreal.EditorLevelLibrary.new_level(full)
            log("Created /Game/Maps/L_Island (fallback path)")
        except Exception as e2:
            warn("Level creation failed: {} / {}".format(e, e2))


# ---------------------------------------------------------------------------
# Project default GameMode + startup map
# ---------------------------------------------------------------------------

def set_project_settings():
    log("Project settings")
    bp_gm = EAS.load_asset("/Game/Blueprints/BP_GameMode")
    try:
        s = unreal.GameMapsSettings.get_default_object()
        if bp_gm and s:
            s.set_editor_property("global_default_game_mode",
                                  unreal.SoftClassPath(bp_gm.generated_class()))
        if EAS.does_asset_exist("/Game/Maps/L_Island"):
            sm = unreal.SoftObjectPath("/Game/Maps/L_Island.L_Island")
            s.set_editor_property("game_default_map", sm)
            s.set_editor_property("editor_startup_map", sm)
        s.save_config()
    except Exception as e:
        warn("Project settings update failed: " + str(e))


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def run():
    log("==== Begin auto-setup ====")
    setup_folders()
    setup_render_targets()
    setup_sound_classes()
    setup_mpc()
    setup_input()
    setup_blueprints()
    setup_datatables()
    wire_defaults()
    setup_level()
    set_project_settings()
    log("==== Done ====")
    log("Next: open IMC_Default and finish the WASD swizzle/negate "
        "modifiers (Docs/01-Player.md).")
    log("Then: drop scene.bin into Content/ImportSource/PirateIslands/ "
        "and run Tools/import_pirate_islands.py.")


if __name__ == "__main__":
    run()
