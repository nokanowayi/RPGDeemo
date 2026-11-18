import json
from pathlib import Path
import unreal

out = {}
registry = unreal.AssetRegistryHelpers.get_asset_registry()
registry.search_all_assets(synchronous_search=True)
for path in [
    "/Game/Characters/UEFN_Mannequin/Animations/Idle/M_Neutral_Crouch_Idle_Loop",
    "/Game/Characters/UEFN_Mannequin/Animations/Traversal/Climb/AM_M_Neutral_Traversal_Climb_Start_2_5_stand_F_Lfoot",
    "/Game/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin",
    "/Game/Assets/Kongxian/IK_KongXian"]:
    asset = unreal.load_asset(path)
    row = {"loaded": bool(asset), "properties": {}}
    if asset:
        for prop in ["notifies", "slot_anim_tracks", "asset_user_data", "post_process_anim_blueprint", "physics_asset", "materials", "preview_skeletal_mesh"]:
            try:
                value = asset.get_editor_property(prop)
                row["properties"][prop] = str(value)[:5000]
            except Exception as e:
                row["properties"][prop] = str(e)
        if isinstance(asset, unreal.AnimSequenceBase):
            row["notify_details"] = []
            for n in unreal.AnimationLibrary.get_animation_notify_events(asset):
                notify = n.get_editor_property("notify") or n.get_editor_property("notify_state_class")
                row["notify_details"].append({"name": str(n.get_editor_property("notify_name")),
                    "class": notify.get_class().get_path_name() if notify else None,
                    "time": unreal.AnimationLibrary.get_anim_notify_event_trigger_time(n),
                    "duration": unreal.AnimationLibrary.get_anim_notify_event_duration(n),
                    "text": notify.export_text() if notify and hasattr(notify,"export_text") else str(notify)})
    out[path] = row
out["controllers"] = {
    "retarget": [x for x in dir(unreal.IKRetargeterController) if "op" in x or "chain" in x],
    "animation_lib": [x for x in dir(unreal.AnimationLibrary) if "notify" in x or "curve" in x],
    "ikrig": [x for x in dir(unreal.IKRigController) if "mesh" in x or "chain" in x]}
Path("E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget/stage_probe.json").write_text(json.dumps(out, indent=2), encoding="utf-8")
