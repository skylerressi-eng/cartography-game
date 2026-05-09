# Copyright (c) 2026 The Last Cartographer.
#
# Imports Content/ImportSource/PirateIslands/scene.gltf into UE5,
# spawns the imported scene actor in the current level, and drops a
# BP_WorldHider next to it so SM_Bld_* / SM_Prop_* meshes start hidden.
#
# Run after Tools/setup_project.py.
#
#   py "Tools/import_pirate_islands.py"

import os
import unreal


EAS = unreal.EditorAssetLibrary
ALT = unreal.AssetToolsHelpers.get_asset_tools()

PROJECT_DIR = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
GLTF_PATH   = os.path.join(PROJECT_DIR, "Content", "ImportSource", "PirateIslands", "scene.gltf")
BIN_PATH    = os.path.join(PROJECT_DIR, "Content", "ImportSource", "PirateIslands", "scene.bin")
DEST_PATH   = "/Game/Maps/Imported/PirateIslands"


def log(msg):  unreal.log("[PirateIslands] " + msg)
def warn(msg): unreal.log_warning("[PirateIslands] " + msg)


def open_island_level():
    full = "/Game/Maps/L_Island"
    if not EAS.does_asset_exist(full):
        warn("L_Island level missing. Run Tools/setup_project.py first.")
        return False
    try:
        ls = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        ls.load_level(full)
    except Exception:
        unreal.EditorLevelLibrary.load_level(full)
    return True


def ensure_bin():
    if not os.path.isfile(BIN_PATH):
        unreal.log_error(
            "Missing scene.bin at {}\n"
            "Copy it from chicken_gun_pirateislands_reupload.zip — see "
            "Content/ImportSource/PirateIslands/README.md".format(BIN_PATH))
        return False
    return True


def already_imported():
    return EAS.does_directory_exist(DEST_PATH) and len(EAS.list_assets(DEST_PATH, recursive=True)) > 0


def import_gltf():
    if already_imported():
        log("glTF already imported, skipping.")
        return
    if not EAS.does_directory_exist(DEST_PATH):
        EAS.make_directory(DEST_PATH)
    task = unreal.AssetImportTask()
    task.filename = GLTF_PATH
    task.destination_path = DEST_PATH
    task.replace_existing = False
    task.automated = True
    task.save = True
    ALT.import_asset_tasks([task])
    log("Imported glTF into " + DEST_PATH)


def find_imported_actor_class():
    """The Interchange importer typically generates either a Blueprint or
    a Level asset for the scene. Return whatever spawnable class we find."""
    for path in EAS.list_assets(DEST_PATH, recursive=True, include_folder=False):
        a = EAS.load_asset(path)
        if isinstance(a, unreal.Blueprint):
            return a.generated_class()
    return None


def spawn_at_origin(asset_class):
    if asset_class is None:
        warn("No spawnable Blueprint produced by the importer; you'll need "
             "to drag the imported assets into the level by hand.")
        return None
    try:
        return unreal.EditorLevelLibrary.spawn_actor_from_class(asset_class, unreal.Vector(0,0,0))
    except Exception as e:
        warn("Spawn failed: " + str(e))
        return None


def spawn_world_hider():
    bp = EAS.load_asset("/Game/Blueprints/BP_WorldHider")
    if bp is None:
        warn("BP_WorldHider missing — run setup_project.py")
        return None
    try:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            bp.generated_class(), unreal.Vector(0, 0, 200))
        log("Spawned BP_WorldHider in level — buildings will hide on Play.")
        return actor
    except Exception as e:
        warn("WorldHider spawn failed: " + str(e))
        return None


def save_level():
    try:
        unreal.EditorLevelLibrary.save_current_level()
    except Exception as e:
        warn("save_current_level failed: " + str(e))


def run():
    log("==== Pirate Islands import ====")
    if not open_island_level(): return
    if not ensure_bin():        return
    import_gltf()
    cls = find_imported_actor_class()
    spawn_at_origin(cls)
    spawn_world_hider()
    save_level()
    log("==== Done. Press Play to walk the islands. ====")


if __name__ == "__main__":
    run()
