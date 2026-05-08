# Copyright (c) 2026 The Last Cartographer.
#
# UE5 Editor Python — auto-create the binary assets that the C++ scaffold
# expects. Run inside the editor:
#
#   1. Open CartographyGame.uproject in UE 5.4 (let it compile).
#   2. Window -> Output Log.
#   3. Switch the bottom-left dropdown from "Cmd" to "Python".
#   4. Type:  py "Tools/setup_project.py"
#
# The script is idempotent — running it twice doesn't duplicate assets.
#
# What it creates under /Game (the in-engine Content/ root):
#   - Folder structure
#   - Render targets (RT_FieldMap, RT_MasterMap, RT_GroundTruth) at 2048
#   - Sound classes (SC_Master/Music/Ambient/SFX)
#   - Material Parameter Collection (MPC_World)
#   - Input Mapping Context + Input Actions
#   - Blueprint subclasses of every C++ class the docs reference
#   - Wiring of defaults (RT into FieldMapComponent, tables into GameMode)
#   - Imports DT_*.csv from /Content/Data/ as DataTables with the right
#     row struct
#
# It does NOT create UMG widgets (their hierarchy is too complex for a
# safe scripted layout) — those still need hand assembly per Docs/13-UI.md.

import unreal


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

EAS = unreal.EditorAssetLibrary
ALT = unreal.AssetToolsHelpers.get_asset_tools()


def log(msg):
    unreal.log("[Cartography setup] " + msg)


def ensure_folder(path):
    """Make sure /Game/<path> exists in the asset tree."""
    if not EAS.does_directory_exist(path):
        EAS.make_directory(path)
        log("Created folder " + path)


def asset_exists(path):
    return EAS.does_asset_exist(path)


def create_asset(name, package_path, asset_class, factory):
    full = package_path + "/" + name
    if asset_exists(full):
        return EAS.load_asset(full)
    asset = ALT.create_asset(name, package_path, asset_class, factory)
    EAS.save_asset(full)
    log("Created asset " + full)
    return asset


def find_class(name):
    """Find a UClass by name (e.g. 'CartographerCharacter')."""
    return unreal.load_class(None, "/Script/CartographyGame." + name)


# ---------------------------------------------------------------------------
# Folder layout
# ---------------------------------------------------------------------------

FOLDERS = [
    "/Game/Maps",
    "/Game/Blueprints",
    "/Game/UI",
    "/Game/Materials",
    "/Game/RenderTargets",
    "/Game/Input",
    "/Game/Pins",
    "/Game/Landmarks",
    "/Game/Audio/SoundClasses",
    "/Game/Audio/Music",
    "/Game/Audio/Ambient",
    "/Game/Data",
    "/Game/MPC",
]


def setup_folders():
    log("Ensuring folder structure")
    for f in FOLDERS:
        ensure_folder(f)


# ---------------------------------------------------------------------------
# Render targets
# ---------------------------------------------------------------------------

def setup_render_targets():
    log("Creating render targets")
    factory = unreal.CanvasRenderTarget2DFactoryNew()
    for name in ("RT_FieldMap", "RT_MasterMap", "RT_GroundTruth"):
        full = "/Game/RenderTargets/" + name
        if asset_exists(full):
            continue
        rt = ALT.create_asset(name, "/Game/RenderTargets",
                              unreal.TextureRenderTarget2D, factory)
        rt.set_editor_property("size_x", 2048)
        rt.set_editor_property("size_y", 2048)
        rt.set_editor_property("render_target_format",
                               unreal.TextureRenderTargetFormat.RTF_RGBA8)
        EAS.save_asset(full)
        log("Created RT " + full)


# ---------------------------------------------------------------------------
# Sound classes
# ---------------------------------------------------------------------------

def setup_sound_classes():
    log("Creating sound classes")
    factory = unreal.SoundClassFactory()
    for name in ("SC_Master", "SC_Music", "SC_Ambient", "SC_SFX"):
        create_asset(name, "/Game/Audio/SoundClasses",
                     unreal.SoundClass, factory)


# ---------------------------------------------------------------------------
# Material parameter collection
# ---------------------------------------------------------------------------

def setup_mpc():
    log("Creating Material Parameter Collection")
    factory = unreal.MaterialParameterCollectionFactoryNew()
    full = "/Game/MPC/MPC_World"
    if asset_exists(full):
        return
    mpc = ALT.create_asset("MPC_World", "/Game/MPC",
                           unreal.MaterialParameterCollection, factory)

    scalars = [
        ("TimeOfDay",       12.0),
        ("WeatherIntensity", 0.0),
        ("Fog",              0.2),
        ("AccuracyGlitch",   0.0),
    ]
    vectors = [
        ("WindDirection", unreal.LinearColor(1, 0, 0, 0)),
        ("BiomeTint",     unreal.LinearColor(1, 1, 1, 1)),
    ]
    s_params = []
    for n, v in scalars:
        p = unreal.CollectionScalarParameter()
        p.set_editor_property("parameter_name", n)
        p.set_editor_property("default_value", v)
        s_params.append(p)
    v_params = []
    for n, v in vectors:
        p = unreal.CollectionVectorParameter()
        p.set_editor_property("parameter_name", n)
        p.set_editor_property("default_value", v)
        v_params.append(p)
    mpc.set_editor_property("scalar_parameters", s_params)
    mpc.set_editor_property("vector_parameters", v_params)
    EAS.save_asset(full)


# ---------------------------------------------------------------------------
# Input
# ---------------------------------------------------------------------------

INPUT_ACTIONS = [
    ("IA_Move",            unreal.InputActionValueType.AXIS2D),
    ("IA_Look",            unreal.InputActionValueType.AXIS2D),
    ("IA_Jump",            unreal.InputActionValueType.BOOLEAN),
    ("IA_ToggleFieldMap",  unreal.InputActionValueType.BOOLEAN),
    ("IA_ToggleNotebook",  unreal.InputActionValueType.BOOLEAN),
    ("IA_Interact",        unreal.InputActionValueType.BOOLEAN),
    ("IA_Draw",            unreal.InputActionValueType.BOOLEAN),
    ("IA_Pause",           unreal.InputActionValueType.BOOLEAN),
    ("IA_QuickSave",       unreal.InputActionValueType.BOOLEAN),
]

KEY_MAP = {
    "IA_Move":           [("W", None), ("A", None), ("S", None), ("D", None)],
    "IA_Look":           [("Mouse XY", None)],
    "IA_Jump":           [("SpaceBar", None)],
    "IA_ToggleFieldMap": [("M", None)],
    "IA_ToggleNotebook": [("N", None)],
    "IA_Interact":       [("E", None)],
    "IA_Draw":           [("LeftMouseButton", None)],
    "IA_Pause":          [("Escape", None)],
    "IA_QuickSave":      [("F5", None)],
}


def setup_input():
    log("Creating Input Actions and Mapping Context")
    ia_factory = unreal.InputActionFactory()
    actions = {}
    for name, value_type in INPUT_ACTIONS:
        full = "/Game/Input/" + name
        if asset_exists(full):
            actions[name] = EAS.load_asset(full)
            continue
        ia = ALT.create_asset(name, "/Game/Input", unreal.InputAction, ia_factory)
        ia.set_editor_property("value_type", value_type)
        EAS.save_asset(full)
        actions[name] = ia
        log("Created " + full)

    imc_factory = unreal.InputMappingContextFactory()
    imc_full = "/Game/Input/IMC_Default"
    if not asset_exists(imc_full):
        imc = ALT.create_asset("IMC_Default", "/Game/Input",
                               unreal.InputMappingContext, imc_factory)
        # Note: building a fully-modified mapping list (with swizzle/negate
        # for WASD axis2D) requires Slate types not all exposed to Python.
        # We add the unmodified mappings; you finish in the editor per
        # Docs/01-Player.md. The mappings are still pre-populated which
        # saves most of the manual clicking.
        for ia_name, keys in KEY_MAP.items():
            ia = actions.get(ia_name)
            if not ia:
                continue
            for key_name, _ in keys:
                imc.map_key(ia, unreal.InputKey(key_name))
        EAS.save_asset(imc_full)
        log("Created " + imc_full)


# ---------------------------------------------------------------------------
# Blueprints
# ---------------------------------------------------------------------------

BP_DEFS = [
    # (BP name, parent C++ class, package path)
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
]


def create_bp(name, parent_class_name, package_path):
    full = package_path + "/" + name
    if asset_exists(full):
        return EAS.load_asset(full)
    parent = find_class(parent_class_name)
    if not parent:
        log("WARN: parent class not found: " + parent_class_name)
        return None
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent)
    bp = ALT.create_asset(name, package_path, unreal.Blueprint, factory)
    EAS.save_asset(full)
    log("Created BP " + full)
    return bp


def setup_blueprints():
    log("Creating Blueprint subclasses")
    for name, parent, path in BP_DEFS:
        create_bp(name, parent, path)


# ---------------------------------------------------------------------------
# DataTable imports
# ---------------------------------------------------------------------------

DATA_TABLES = [
    ("DT_Regions",      "FRegionRow"),
    ("DT_Biomes",       "FBiomeRow"),
    ("DT_Tools",        "FToolRow"),
    ("DT_Landmarks",    "FLandmarkRow"),
    ("DT_Lore",         "FLoreRow"),
    ("DT_Achievements", "FAchievementRow"),
]


def import_csv_as_datatable(csv_disk_path, asset_name, row_struct_name):
    full = "/Game/Data/" + asset_name
    if asset_exists(full):
        return EAS.load_asset(full)
    task = unreal.AssetImportTask()
    task.filename = csv_disk_path
    task.destination_path = "/Game/Data"
    task.destination_name = asset_name
    task.replace_existing = True
    task.automated = True
    task.save = True

    ud = unreal.CSVImportSettings()
    ud.import_type = unreal.CSVImportType.ECSV_DATA_TABLE
    struct = unreal.load_object(None,
        "/Script/CartographyGame." + row_struct_name)
    if struct:
        ud.import_row_struct = struct
    task.options = ud

    ALT.import_asset_tasks([task])
    log("Imported " + full)
    return EAS.load_asset(full)


def setup_datatables():
    log("Importing DataTables")
    project_dir = unreal.Paths.convert_relative_path_to_full(
        unreal.Paths.project_dir())
    for name, row_struct in DATA_TABLES:
        csv = project_dir + "Content/Data/" + name + ".csv"
        if not unreal.Paths.file_exists(csv):
            log("WARN: missing csv: " + csv)
            continue
        import_csv_as_datatable(csv, name, row_struct)


# ---------------------------------------------------------------------------
# Wire up defaults on the spawned BPs
# ---------------------------------------------------------------------------

def wire_blueprint_defaults():
    log("Wiring Blueprint defaults")

    rt_field   = EAS.load_asset("/Game/RenderTargets/RT_FieldMap")
    rt_master  = EAS.load_asset("/Game/RenderTargets/RT_MasterMap")
    rt_truth   = EAS.load_asset("/Game/RenderTargets/RT_GroundTruth")
    imc        = EAS.load_asset("/Game/Input/IMC_Default")

    # BP_Cartographer: input + render targets on FieldMap component
    bp = EAS.load_asset("/Game/Blueprints/BP_Cartographer")
    if bp:
        cdo = unreal.get_default_object(bp.generated_class())
        if cdo:
            cdo.set_editor_property("default_mapping_context", imc)
            for name in ("MoveAction", "LookAction", "JumpAction",
                         "ToggleFieldMapAction", "ToggleNotebookAction",
                         "InteractAction", "DrawAction"):
                ia_name = "IA_" + name.replace("Action", "")
                ia = EAS.load_asset("/Game/Input/" + ia_name)
                if ia:
                    cdo.set_editor_property(name[0].lower() + name[1:], ia)

            field_map = cdo.get_editor_property("FieldMap")
            if field_map:
                field_map.set_editor_property("FieldMapRT", rt_field)
                field_map.set_editor_property("MasterMapRT", rt_master)
        EAS.save_asset(bp.get_path_name())

    # BP_GameMode: data tables + default classes
    bp_gm = EAS.load_asset("/Game/Blueprints/BP_GameMode")
    if bp_gm:
        cdo = unreal.get_default_object(bp_gm.generated_class())
        if cdo:
            for table_name, prop_name in [
                ("DT_Regions",      "RegionsTable"),
                ("DT_Biomes",       "BiomesTable"),
                ("DT_Tools",        "ToolsTable"),
                ("DT_Landmarks",    "LandmarksTable"),
                ("DT_Lore",         "LoreTable"),
                ("DT_Achievements", "AchievementsTable"),
            ]:
                table = EAS.load_asset("/Game/Data/" + table_name)
                if table:
                    cdo.set_editor_property(prop_name, table)
            pawn = EAS.load_asset("/Game/Blueprints/BP_Cartographer")
            pc   = EAS.load_asset("/Game/Blueprints/BP_PlayerController")
            if pawn:
                cdo.set_editor_property("default_pawn_class",
                                        pawn.generated_class())
            if pc:
                cdo.set_editor_property("player_controller_class",
                                        pc.generated_class())
        EAS.save_asset(bp_gm.get_path_name())

    # BP_GroundTruthCapture: assign truth render target
    bp_gt = EAS.load_asset("/Game/Blueprints/BP_GroundTruthCapture")
    if bp_gt:
        cdo = unreal.get_default_object(bp_gt.generated_class())
        if cdo:
            cdo.set_editor_property("GroundTruthRT", rt_truth)
        EAS.save_asset(bp_gt.get_path_name())


# ---------------------------------------------------------------------------
# Project Settings: default game mode
# ---------------------------------------------------------------------------

def set_project_default_gamemode():
    log("Setting default GameMode in Project Settings")
    bp_gm = EAS.load_asset("/Game/Blueprints/BP_GameMode")
    if not bp_gm:
        return
    settings = unreal.GameMapsSettings.get_default_object()
    if settings:
        settings.set_editor_property("global_default_game_mode",
                                     unreal.SoftClassPath(bp_gm.generated_class()))
    unreal.SystemLibrary.execute_console_command(
        unreal.EditorLevelLibrary.get_editor_world(),
        "SaveConfig")


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
    wire_blueprint_defaults()
    set_project_default_gamemode()
    log("==== Done ====")
    log("Open BP_Cartographer, finish IMC_Default mappings (WASD swizzle),")
    log("create UMG widget assets per Docs/13-UI.md, and place a level.")


if __name__ == "__main__":
    run()
