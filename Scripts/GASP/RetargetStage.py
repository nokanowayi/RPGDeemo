"""Bake only in the disposable GASPStage project, never in either live project.

Run PrepareStage.ps1 first. DeliverStage.ps1 copies only the validated output closure.
"""
import json
from pathlib import Path
import unreal

REPORT_DIR = Path("E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget")
ROOT = "/Game/Assets/Kongxian/HeroAnimation/GASP"
SOURCE = "/Game/Characters/UEFN_Mannequin"
TARGET = "/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady"
RETARGETER = ROOT + "/Rigs/RTG_UEFN_Kongxian_GASP"
LIB = unreal.AnimationLibrary


def save(asset):
    assert unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False), asset.get_path_name()


def notify_info(asset):
    result = []
    for event in LIB.get_animation_notify_events(asset):
        obj = event.get_editor_property("notify") or event.get_editor_property("notify_state_class")
        item = {"name": str(event.get_editor_property("notify_name")),
                "class": obj.get_class().get_path_name() if obj else None,
                "time": LIB.get_anim_notify_event_trigger_time(event),
                "duration": LIB.get_anim_notify_event_duration(event)}
        if obj and obj.get_class().get_name() == "AnimNotifyState_MotionWarping":
            modifier = obj.get_editor_property("root_motion_modifier")
            item["warp_target"] = str(modifier.get_editor_property("warp_target_name")) if modifier else None
        result.append(item)
    return result


def metadata(asset):
    item = {"duration": asset.get_play_length(), "notifies": notify_info(asset),
            "curves": [str(n) for n in LIB.get_animation_curve_names(asset, unreal.RawCurveTrackTypes.RCT_FLOAT)]}
    if isinstance(asset, unreal.AnimSequence):
        for prop in ["enable_root_motion", "force_root_lock", "root_motion_root_lock", "additive_anim_type", "rate_scale"]:
            item[prop] = str(asset.get_editor_property(prop))
    return item


def main():
    assert Path(unreal.Paths.project_dir()).resolve().name == "Stage", "This script MUST run in the isolated Stage project"
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    registry.search_all_assets(synchronous_search=True)
    inventory = json.loads((REPORT_DIR / "source_inventory.json").read_text(encoding="utf-8"))
    rows = [r for r in inventory["selected"] if r["class"] in ["AnimSequence", "AnimMontage"]]
    assert len(rows) == 364, len(rows)
    assets = [unreal.load_asset(r["package"]) for r in rows]
    assert all(assets)
    source_meta = {r["name"]: metadata(a) for r, a in zip(rows, assets)}
    (REPORT_DIR / "source_metadata.json").write_text(json.dumps(source_meta, indent=2), encoding="utf-8")
    source_mesh = unreal.load_asset(SOURCE + "/Meshes/SKM_UEFN_Mannequin")
    target_mesh = unreal.load_asset(TARGET)
    source_rig = unreal.load_asset(SOURCE + "/Rigs/IK_UEFN_Mannequin")
    target_rig = unreal.load_asset("/Game/Assets/Kongxian/IK_KongXian")
    target_skeleton = target_mesh.get_editor_property("skeleton")
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.IKRetargetFactory()
    factory.set_editor_property("edit_after_new", False)
    retargeter = unreal.load_asset(RETARGETER) if unreal.EditorAssetLibrary.does_asset_exist(RETARGETER) else tools.create_asset("RTG_UEFN_Kongxian_GASP", ROOT + "/Rigs", unreal.IKRetargeter, factory)
    controller = unreal.IKRetargeterController.get_controller(retargeter)
    controller.set_ik_rig(unreal.RetargetSourceOrTarget.SOURCE, source_rig)
    controller.set_ik_rig(unreal.RetargetSourceOrTarget.TARGET, target_rig)
    controller.set_preview_mesh(unreal.RetargetSourceOrTarget.SOURCE, source_mesh)
    controller.set_preview_mesh(unreal.RetargetSourceOrTarget.TARGET, target_mesh)
    controller.remove_all_ops()
    controller.add_default_ops()
    for index in range(controller.get_num_retarget_ops()):
        controller.run_op_initial_setup(index)
    controller.auto_map_chains(unreal.AutoMapChainType.EXACT, True)
    controller.auto_align_all_bones(unreal.RetargetSourceOrTarget.TARGET, unreal.RetargetAutoAlignMethod.CHAIN_TO_CHAIN)
    mapping = {str(c.chain_name): str(controller.get_source_chain(c.chain_name))
               for c in unreal.IKRigController.get_controller(target_rig).get_retarget_chains()}
    for name in ["Spine", "Neck", "Head", "LeftArm", "RightArm", "LeftLeg", "RightLeg"]:
        assert mapping.get(name) not in [None, "", "None"], (name, mapping)
    save(retargeter)
    existing = [d for d in registry.get_assets_by_path(ROOT, recursive=True)
                if str(d.asset_class_path.asset_name) in ["AnimSequence", "AnimMontage"]]
    assert len(existing) in [0, 364], "Partial output requires manual audit; refusing to replace"
    baked = existing or unreal.IKRetargetBatchOperation.duplicate_and_retarget(
        [unreal.EditorAssetLibrary.find_asset_data(a.get_path_name()) for a in assets],
        source_mesh, target_mesh, retargeter,
        prefix="KX_GASP_", include_referenced_assets=False)
    assert len(baked) == 364, len(baked)
    by_name = {str(d.asset_name): d.get_asset() for d in baked}
    renames = []
    for row in rows:
        source_relative = row["package"].split("/Animations/")[1]
        group, relative = source_relative.split("/", 1)
        if group == "Idle":
            group, relative = "Crouch", "Idle/" + relative
        relative_dir = relative.rsplit("/", 1)[0] if "/" in relative else ""
        kind = "Montages" if row["class"] == "AnimMontage" else "Sequences"
        directory = f"{ROOT}/{kind}/{group}" + ("/" + relative_dir if relative_dir else "")
        row["target"] = directory + "/KX_GASP_" + row["name"]
        asset = by_name["KX_GASP_" + row["name"]]
        if asset.get_path_name().split(".")[0] != row["target"]:
            renames.append(unreal.AssetRenameData(asset, directory, "KX_GASP_" + row["name"]))
    if renames:
        assert tools.rename_assets(renames), "Failed to move retargeted assets"

    # Preserve the small, standalone early-blend-out implementation. The stock
    # PoseSearch and Foley events are not connected to this project's anim graph.
    def support_asset(new, old):
        return unreal.load_asset(new) if unreal.EditorAssetLibrary.does_asset_exist(new) else unreal.load_asset(old)
    blend_bp = unreal.load_asset("/Game/Blueprints/AnimNotifies/BP_NotifyState_MontageBlendOut")
    blend_enum = unreal.load_asset("/Game/Blueprints/AnimNotifies/E_TraversalBlendOutCondition")
    compression = support_asset(ROOT + "/Settings/GASP_AnimCurveCompressionSettings", "/Game/Misc/SandboxAnimCurveCompressionSettings")
    # Generated Blueprint classes do not reliably remap when the BP is moved in
    # a commandlet. Keep the two standalone notify packages at their source paths.
    support = [(compression, ROOT + "/Settings", "GASP_AnimCurveCompressionSettings")]
    assert blend_bp and blend_enum
    assert all(a for a, _, _ in support)
    support_renames = [unreal.AssetRenameData(a, d, n) for a, d, n in support if a.get_path_name().split(".")[0] != d + "/" + n]
    if support_renames:
        assert tools.rename_assets(support_renames)
    unreal.BlueprintEditorLibrary.compile_blueprint(blend_bp)
    for asset, _, _ in support:
        save(asset)

    report = {"source_project": inventory["project_dir"], "target_mesh": TARGET,
              "retargeter": RETARGETER, "chain_mapping": mapping, "assets": [],
              "notify_policy": "Keep MotionWarping and standalone MontageBlendOut; remove GASP Foley, PoseSearch, EarlyTransition events; preserve curves and sync markers",
              "removed_events": [], "removed_user_data": [], "removed_blend_profiles": []}
    for row in rows:
        asset = unreal.load_asset(row["target"])
        before = source_meta[row["name"]]
        remove_names = set()
        for event in notify_info(asset):
            cls = event["class"] or ""
            keep = (not cls or cls.startswith("/Script/Engine.")
                    or cls.startswith("/Script/MotionWarping.")
                    or "MontageBlendOut" in cls)
            if not keep:
                remove_names.add(event["name"])
                report["removed_events"].append({"asset": row["target"], **event})
        for name in remove_names:
            assert LIB.remove_animation_notify_events_by_name(asset, name) > 0
        userdata = asset.get_editor_property("asset_user_data")
        if userdata:
            report["removed_user_data"].append({"asset": row["target"], "classes": [u.get_class().get_path_name() for u in userdata]})
            asset.set_editor_property("asset_user_data", [])
        if isinstance(asset, unreal.AnimSequence):
            asset.set_editor_property("curve_compression_settings", compression)
        else:
            for prop in ["blend_profile_in", "blend_profile_out"]:
                value = asset.get_editor_property(prop)
                if value:
                    report["removed_blend_profiles"].append({"asset": row["target"], "property": prop, "value": value.get_path_name()})
                    asset.set_editor_property(prop, None)
        asset.set_preview_skeletal_mesh(target_mesh)
        assert asset.get_editor_property("skeleton") == target_skeleton
        after = metadata(asset)
        assert abs(after["duration"] - before["duration"]) < 1e-5
        assert sorted(n.lower() for n in after["curves"]) == sorted(n.lower() for n in before["curves"]), (row["name"], before["curves"], after["curves"])
        for prop in ["enable_root_motion", "force_root_lock", "root_motion_root_lock", "additive_anim_type", "rate_scale"]:
            if prop in before:
                assert before[prop] == after[prop], (row["name"], prop, before[prop], after[prop])
        warp_before = [n for n in before["notifies"] if n["class"] == "/Script/MotionWarping.AnimNotifyState_MotionWarping"]
        warp_after = [n for n in after["notifies"] if n["class"] == "/Script/MotionWarping.AnimNotifyState_MotionWarping"]
        assert warp_before == warp_after, row["name"]
        save(asset)
        report["assets"].append({**row, **after})
        if len(report["assets"]) % 20 == 0:
            (REPORT_DIR / "retarget_result.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
            unreal.log("GASP_BAKE_SAVED " + str(len(report["assets"])))

    # Editor-only source preview assets, cleaned and moved under the output root.
    # Only the disposable copies are edited, never the source GASP mesh.
    source_skeleton = source_mesh.get_editor_property("skeleton")
    source_mesh.set_editor_property("post_process_anim_blueprint", None)
    source_mesh.set_editor_property("physics_asset", None)
    materials = source_mesh.get_editor_property("materials")
    for material in materials:
        material.set_editor_property("material_interface", unreal.load_asset("/Engine/EngineMaterials/DefaultMaterial"))
        material.set_editor_property("overlay_material_interface", None)
    source_mesh.set_editor_property("materials", materials)
    source_skeleton.set_editor_property("asset_user_data", [])
    rig_support = [(source_mesh, ROOT + "/Rigs/Source", "SKM_GASP_UEFN_Source"),
                   (source_skeleton, ROOT + "/Rigs/Source", "SK_GASP_UEFN_Source"),
                   (source_rig, ROOT + "/Rigs/Source", "IK_GASP_UEFN_Source")]
    assert tools.rename_assets([unreal.AssetRenameData(a, d, n) for a, d, n in rig_support])
    for asset, _, _ in rig_support:
        save(asset)
    save(retargeter)
    report["sequence_count"] = sum(r["class"] == "AnimSequence" for r in rows)
    report["montage_count"] = sum(r["class"] == "AnimMontage" for r in rows)
    report["complete"] = True
    (REPORT_DIR / "retarget_result.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
    print("GASP_BAKE_COMPLETE " + json.dumps({"sequences": report["sequence_count"], "montages": report["montage_count"]}))
    (REPORT_DIR / "retarget_error.txt").unlink(missing_ok=True)


try:
    main()
except Exception:
    import traceback
    (REPORT_DIR / "retarget_error.txt").write_text(traceback.format_exc(), encoding="utf-8")
    unreal.log_error(traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
