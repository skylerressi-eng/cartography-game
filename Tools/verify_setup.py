# Copyright (c) 2026 The Last Cartographer.
#
# Run AFTER bootstrap.py to verify everything got created and wired.
#
#     py "Tools/verify_setup.py"

import unreal


EAS = unreal.EditorAssetLibrary
issues = []


def check(asset_path):
    if not EAS.does_asset_exist(asset_path):
        issues.append("MISSING: " + asset_path)
        return None
    return EAS.load_asset(asset_path)


REQUIRED_ASSETS = [
    "/Game/Maps/L_Island",
    "/Game/RenderTargets/RT_FieldMap",
    "/Game/RenderTargets/RT_MasterMap",
    "/Game/RenderTargets/RT_GroundTruth",
    "/Game/Materials/M_PencilBrush",
    "/Game/Materials/M_EraserBrush",
    "/Game/Materials/M_InkBrush",
    "/Game/Input/IMC_Default",
    "/Game/Input/IA_Move",
    "/Game/Input/IA_Look",
    "/Game/Input/IA_Draw",
    "/Game/Blueprints/BP_Cartographer",
    "/Game/Blueprints/BP_GameMode",
    "/Game/Blueprints/BP_PlayerController",
    "/Game/Blueprints/BP_WorldHider",
    "/Game/Blueprints/BP_PirateIslandWorld",
    "/Game/Data/DT_Regions",
    "/Game/Data/DT_Biomes",
    "/Game/Data/DT_Tools",
    "/Game/Data/DT_Landmarks",
    "/Game/Data/DT_Lore",
    "/Game/Data/DT_Achievements",
    "/Game/UI/WBP_HUD",
    "/Game/UI/WBP_FieldMap",
    "/Game/UI/WBP_Notebook",
    "/Game/UI/WBP_Inking",
    "/Game/UI/WBP_Pause",
    "/Game/UI/WBP_Compass",
    "/Game/UI/WBP_DiscoveryToast",
    "/Game/MPC/MPC_World",
]


def run():
    unreal.log("==== Verifying Cartography setup ====")
    for p in REQUIRED_ASSETS:
        check(p)

    bp_pawn = check("/Game/Blueprints/BP_Cartographer")
    if bp_pawn:
        try:
            cdo = unreal.get_default_object(bp_pawn.generated_class())
            fm = cdo.get_editor_property("FieldMap") if cdo else None
            if fm:
                if fm.get_editor_property("FieldMapRT") is None:
                    issues.append("BP_Cartographer.FieldMap.FieldMapRT not assigned")
                if fm.get_editor_property("PencilBrushMaterial") is None:
                    issues.append("BP_Cartographer.FieldMap.PencilBrushMaterial not assigned")
        except Exception as e:
            issues.append("Cannot inspect BP_Cartographer: " + str(e))

    if issues:
        unreal.log_warning("==== Issues ====")
        for i in issues:
            unreal.log_warning("  " + i)
        unreal.log_warning("Re-run Tools/bootstrap.py to fill in missing pieces.")
    else:
        unreal.log("==== All checks passed. Press Play. ====")


if __name__ == "__main__":
    run()
