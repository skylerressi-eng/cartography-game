# Copyright (c) 2026 The Last Cartographer.
#
# Imports Content/ImportSource/PirateIslands/scene.gltf into UE5,
# spawns a BP_PirateIslandWorld in the current level, and applies the
# BuildingHider so SM_Bld_* / SM_Prop_* sub-meshes start hidden.
#
# Run from the editor:
#   Window -> Output Log -> dropdown to Python -> py "Tools/import_pirate_islands.py"
#
# Prereqs:
#   - scene.bin sitting next to scene.gltf (see ImportSource README).
#   - The C++ class APirateIslandWorld and UBuildingHiderComponent must
#     be compiled.

import os
import unreal


EAS = unreal.EditorAssetLibrary
ALT = unreal.AssetToolsHelpers.get_asset_tools()
EUL = unreal.EditorLevelLibrary

PROJECT_DIR = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
GLTF_PATH   = os.path.join(PROJECT_DIR, "Content", "ImportSource", "PirateIslands", "scene.gltf")
BIN_PATH    = os.path.join(PROJECT_DIR, "Content", "ImportSource", "PirateIslands", "scene.bin")
DEST_PATH   = "/Game/Maps/Imported/PirateIslands"


def log(msg): unreal.log("[PirateIslands import] " + msg)


def ensure_bin():
    if not os.path.isfile(BIN_PATH):
        unreal.log_error(
            "Missing scene.bin at " + BIN_PATH + "\n"
            "Copy it from the original chicken_gun_pirateislands_reupload.zip "
            "(see Content/ImportSource/PirateIslands/README.md).")
        return False
    return True


def ensure_dest():
    if not EAS.does_directory_exist(DEST_PATH):
        EAS.make_directory(DEST_PATH)


def import_gltf():
    """
    Import the glTF. UE 5.4 ships with the GLTFImporter / Interchange
    framework; we use the generic AssetImportTask so the script works
    regardless of which path is enabled.
    """
    if any(EAS.does_asset_exist(DEST_PATH + "/" + n) for n in ("scene", "Sketchfab_model")):
        log("Already imported, skipping.")
        return

    task = unreal.AssetImportTask()
    task.filename = GLTF_PATH
    task.destination_path = DEST_PATH
    task.replace_existing = False
    task.automated = True
    task.save = True
    ALT.import_asset_tasks([task])
    log("Imported glTF into " + DEST_PATH)


def find_island_blueprint():
    """Return the BP_PirateIslandWorld asset if the setup script created one."""
    p = "/Game/Blueprints/BP_PirateIslandWorld"
    return EAS.load_asset(p) if EAS.does_asset_exist(p) else None


def find_imported_root_actor_class():
    """
    glTF/Interchange import typically produces either a Static Mesh per
    primitive plus a Level/Blueprint that arranges them, or one large
    Static Mesh. We look for a generated BP whose name starts with
    "Sketchfab" or "scene" inside the destination folder.
    """
    candidates = EAS.list_assets(DEST_PATH, recursive=True, include_folder=False)
    for path in candidates:
        if path.endswith("_C") or "Blueprint" in path:
            asset = EAS.load_asset(path)
            if isinstance(asset, unreal.Blueprint):
                return asset.generated_class()
    return None


def spawn_world_actor():
    bp_island = find_island_blueprint()
    if bp_island is None:
        log("BP_PirateIslandWorld not found — run setup_project.py first.")
        return None
    actor = EUL.spawn_actor_from_class(bp_island.generated_class(),
                                       unreal.Vector(0, 0, 0))
    if actor is None:
        log("Spawn failed.")
        return None
    log("Spawned BP_PirateIslandWorld in current level.")
    return actor


def attach_imported_meshes(world_actor):
    """
    For each imported StaticMesh in DEST_PATH, add a UStaticMeshComponent
    to the spawned BP_PirateIslandWorld actor. Heavy but only runs once.
    Names are preserved so the BuildingHider can match SM_Bld_/SM_Prop_.
    """
    if world_actor is None:
        return
    meshes = []
    for path in EAS.list_assets(DEST_PATH, recursive=True, include_folder=False):
        a = EAS.load_asset(path)
        if isinstance(a, unreal.StaticMesh):
            meshes.append(a)
    log("Attaching {} static meshes (this may take a moment)".format(len(meshes)))
    for mesh in meshes:
        comp_name = mesh.get_name()
        comp = unreal.GameplayStatics.spawn_object(unreal.StaticMeshComponent, world_actor)
        if comp is None: continue
        comp.set_static_mesh(mesh)
        comp.attach_to_component(world_actor.root_component, "",
                                 unreal.AttachmentRule.KEEP_RELATIVE,
                                 unreal.AttachmentRule.KEEP_RELATIVE,
                                 unreal.AttachmentRule.KEEP_RELATIVE,
                                 False)
        comp.rename(comp_name)


def apply_hider(world_actor):
    if world_actor is None: return
    hider = world_actor.get_component_by_class(unreal.BuildingHiderComponent)
    if hider:
        hider.set_editor_property("bApplyOnBeginPlay", True)
        # In-editor preview:
        hider.call_method("ApplyToOwner")
        log("BuildingHider applied (Bld/Prop hidden).")


def update_gamemode_bounds(world_actor):
    """Push the imported island's bounds into BP_GameMode defaults."""
    if world_actor is None: return
    bp_gm = EAS.load_asset("/Game/Blueprints/BP_GameMode")
    if not bp_gm: return
    cdo = unreal.get_default_object(bp_gm.generated_class())
    if not cdo: return
    bounds = world_actor.call_method("ComputeWorldBounds")
    extent = bounds.get_size() if bounds else None
    if extent and extent.x > 0:
        cdo.set_editor_property("WorldOriginXY",
            unreal.Vector2D(-extent.x * 0.5, -extent.y * 0.5))
        cdo.set_editor_property("WorldSizeXY",
            unreal.Vector2D(extent.x, extent.y))
        EAS.save_asset(bp_gm.get_path_name())
        log("Updated BP_GameMode bounds to ({}, {}) cm.".format(int(extent.x), int(extent.y)))


def run():
    log("=== Pirate Islands import ===")
    if not ensure_bin(): return
    ensure_dest()
    import_gltf()
    actor = spawn_world_actor()
    if actor is not None:
        attach_imported_meshes(actor)
        apply_hider(actor)
        update_gamemode_bounds(actor)
        EUL.save_current_level()
    log("Done. Hidden buildings can be revealed by toggling the Hider's "
        "bReveal flag and calling ApplyToOwner from BP/Console.")


if __name__ == "__main__":
    run()
