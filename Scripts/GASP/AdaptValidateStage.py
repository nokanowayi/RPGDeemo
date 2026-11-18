"""Validate baked output and adapt missing UEFN warp helper bones in staging.

Static points are calibrated starting values, not a claim of obstacle contact QA.
The source attach point offset is transported in root axes from its nearest
hand/foot reference to the corresponding retargeted bone at the window end.
"""
import json
import math
from pathlib import Path
import unreal

REPORT_DIR = Path("E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget")
ROOT = "/Game/Assets/Kongxian/HeroAnimation/GASP"
P = unreal.AnimPoseExtensions
L = unreal.AnimationLibrary
T = unreal.GASPRetargetToolsLibrary


def xyz(v):
    return [v.x, v.y, v.z]


def pose_at(sequence, time):
    options = unreal.AnimPoseEvaluationOptions(
        evaluation_type=unreal.AnimDataEvalType.RAW,
        extract_root_motion=False, incorporate_root_motion_into_pose=True,
        should_retarget=False, evaluate_curves=True)
    pose = P.get_anim_pose_at_time(sequence, min(max(time, 0.0), sequence.get_play_length()), options)
    assert P.is_valid(pose), sequence.get_path_name()
    return pose


def bone(pose, name):
    assert name in [str(n) for n in P.get_bone_names(pose)], name
    return P.get_bone_pose(pose, name, unreal.AnimPoseSpaces.WORLD)


def segment_at(description, montage_time):
    tracks = description["slots"]
    assert len(tracks) == 1
    for segment in tracks[0]["segments"]:
        rate = segment["play_rate"]
        assert rate > 0, "Reverse segments require explicit adaptation"
        duration = (segment["anim_end_time"] - segment["anim_start_time"]) / rate
        relative = montage_time - segment["start_pos"]
        if -1e-4 <= relative <= duration * segment["loop_count"] + 1e-4:
            cycle_time = relative % duration
            if relative > 0 and abs(cycle_time) < 1e-5:
                cycle_time = duration
            return unreal.load_asset(segment["animation"]), segment["anim_start_time"] + cycle_time * rate
    raise AssertionError((description["asset"], montage_time))


def norm(v):
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)


def main():
    assert Path(unreal.Paths.project_dir()).resolve().name == "Stage"
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    registry.search_all_assets(synchronous_search=True)
    baked = json.loads((REPORT_DIR / "retarget_result.json").read_text(encoding="utf-8"))
    assert baked["complete"] and len(baked["assets"]) == 364
    target_skeleton = unreal.load_asset("/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady_Skeleton")
    results = {"assets": [], "montages": [], "warp_adaptations": [],
               "pose_sample_count": 0, "finite_transform_count": 0,
               "contact_calibration": "Static warp points mapped from source attach offset at window end; final obstacle/hand contact requires gameplay integration QA"}
    for row in baked["assets"]:
        target = unreal.load_asset(row["target"])
        source = unreal.load_asset(row["package"])
        assert target and source, row["target"]
        assert target.get_editor_property("skeleton") == target_skeleton
        assert abs(target.get_play_length() - source.get_play_length()) < 1e-5
        target_curve_names = {str(n).lower(): n for n in L.get_animation_curve_names(target, unreal.RawCurveTrackTypes.RCT_FLOAT)}
        source_curve_names = {str(n).lower(): n for n in L.get_animation_curve_names(source, unreal.RawCurveTrackTypes.RCT_FLOAT)}
        assert target_curve_names.keys() == source_curve_names.keys(), row["target"]
        for name in target_curve_names:
            source_keys = L.get_float_keys(source, source_curve_names[name])
            target_keys = L.get_float_keys(target, target_curve_names[name])
            assert len(source_keys[0]) == len(target_keys[0]) and len(source_keys[1]) == len(target_keys[1])
            assert all(abs(a-b) < 1e-4 for a,b in zip(source_keys[0], target_keys[0]))
            assert all(abs(a-b) < 1e-4 for a,b in zip(source_keys[1], target_keys[1])), (row["target"], name)
        if isinstance(target, unreal.AnimSequence):
            assert str(L.get_animation_sync_markers(target)).lower() == str(L.get_animation_sync_markers(source)).lower(), row["target"]
            root_samples = []
            for ratio in [0.0, 0.25, 0.5, 0.75, 1.0]:
                time = target.get_play_length() * ratio
                pose = pose_at(target, time)
                for name in ["root", "pelvis", "head", "hand_l", "hand_r", "foot_l", "foot_r"]:
                    tf = bone(pose, name)
                    assert all(math.isfinite(x) for x in xyz(tf.translation) + xyz(tf.scale3d) + [tf.rotation.x,tf.rotation.y,tf.rotation.z,tf.rotation.w])
                    assert max(abs(x) for x in xyz(tf.translation)) < 100000
                    results["finite_transform_count"] += 1
                source_pose = pose_at(source, time)
                root_samples.append({"time": time, "source": xyz(bone(source_pose,"root").translation),
                                     "target": xyz(bone(pose,"root").translation)})
                results["pose_sample_count"] += 1
            s = unreal.Vector(*[root_samples[-1]["source"][i]-root_samples[0]["source"][i] for i in range(3)])
            t = unreal.Vector(*[root_samples[-1]["target"][i]-root_samples[0]["target"][i] for i in range(3)])
            root = {"source_delta": xyz(s), "target_delta": xyz(t), "samples": root_samples}
            if norm(s) > 1:
                root["scale"] = norm(t) / norm(s)
                root["direction_dot"] = (s.x*t.x+s.y*t.y+s.z*t.z)/(norm(s)*max(norm(t),1e-9))
                assert 0.2 < root["scale"] < 2.0 and root["direction_dot"] > 0.99, (row["target"],root)
            results["assets"].append({"asset": row["target"], "curves_equal": True, "sync_markers_equal": True, "root": root})
        else:
            assert T.set_montage_slot(target, "FullBodySlot"), row["target"]
            assert T.restore_blend_out_notify(source, target), row["target"]
            before = json.loads(T.describe_montage(source))
            after = json.loads(T.describe_montage(target))
            assert before["sections"] == after["sections"], row["target"]
            for prop in ["blend_in", "blend_out", "blend_out_trigger_time", "enable_auto_blend_out", "blend_mode_in", "blend_mode_out", "rate_scale"]:
                assert before[prop] == after[prop], (row["target"], prop)
            assert len(before["slots"]) == len(after["slots"])
            for a,b in zip(before["slots"],after["slots"]):
                assert b["name"] == "FullBodySlot"
                assert len(a["segments"]) == len(b["segments"])
                for ss,tt in zip(a["segments"],b["segments"]):
                    assert tt["animation"].startswith(ROOT + "/Sequences/"), tt
                    assert all(ss[k] == tt[k] for k in ss if k != "animation")
            source_events = L.get_animation_notify_events(source)
            events = L.get_animation_notify_events(target)
            source_warps = [n for n in source_events if isinstance(n.get_editor_property("notify_state_class"),unreal.AnimNotifyState_MotionWarping)]
            target_warps = [n for n in events if isinstance(n.get_editor_property("notify_state_class"),unreal.AnimNotifyState_MotionWarping)]
            assert len(source_warps) == len(target_warps)
            for se,te in zip(source_warps,target_warps):
                sm = se.get_editor_property("notify_state_class").get_editor_property("root_motion_modifier")
                tm = te.get_editor_property("notify_state_class").get_editor_property("root_motion_modifier")
                for prop in ["warp_target_name","warp_translation","ignore_z_axis","warp_to_feet_location","warp_rotation","rotation_type","rotation_method","warp_rotation_time_multiplier","warp_max_rotation_rate","subtract_remaining_root_motion"]:
                    assert sm.get_editor_property(prop) == tm.get_editor_property(prop), (row["target"],prop)
                if sm.get_editor_property("warp_point_anim_provider") == unreal.WarpPointAnimProvider.BONE:
                    endpoint = L.get_anim_notify_event_trigger_time(se) + L.get_anim_notify_event_duration(se)
                    source_sequence, source_time = segment_at(before, endpoint)
                    target_sequence, target_time = segment_at(after, endpoint)
                    sp, tp = pose_at(source_sequence,source_time), pose_at(target_sequence,target_time)
                    helper = str(sm.get_editor_property("warp_point_anim_bone_name"))
                    source_point = bone(sp,helper)
                    sr, tr = bone(sp,"root"), bone(tp,"root")
                    source_height = P.get_ref_bone_pose(sp,"pelvis",unreal.AnimPoseSpaces.WORLD).translation.z
                    target_height = P.get_ref_bone_pose(tp,"pelvis",unreal.AnimPoseSpaces.WORLD).translation.z
                    scale = target_height / source_height
                    candidates = ["hand_l","hand_r","foot_l","foot_r","pelvis"]
                    anchor = min(candidates,key=lambda n:norm(source_point.translation - bone(sp,n).translation))
                    source_anchor, target_anchor = bone(sp,anchor), bone(tp,anchor)
                    # Express the source helper offset in source root axes, then
                    # put it in target root axes at the target clip's EndTime.
                    offset = sr.rotation.inversed().rotate_vector(source_point.translation-source_anchor.translation) * scale
                    location = target_anchor.translation + tr.rotation.rotate_vector(offset)
                    rotation = tr.rotation * sr.rotation.inversed() * source_point.rotation
                    static = unreal.Transform(location=location,rotation=rotation.rotator(),scale=unreal.Vector(1,1,1))
                    tm.set_editor_property("warp_point_anim_provider",unreal.WarpPointAnimProvider.STATIC)
                    tm.set_editor_property("warp_point_anim_transform",static)
                    tm.set_editor_property("warp_point_anim_bone_name","None")
                    results["warp_adaptations"].append({"montage":row["target"],"target":str(tm.get_editor_property("warp_target_name")),
                        "time":endpoint,"sequence_time":target_time,"source_helper":helper,"anchor":anchor,"height_scale":scale,
                        "static_translation":xyz(location),"static_rotation":str(rotation),"source_helper_translation":xyz(source_point.translation)})
            assert unreal.EditorAssetLibrary.save_loaded_asset(target,only_if_is_dirty=False)
            restored = json.loads(T.describe_notifies(target))
            assert all(not n["notify_state"].get("is_null",False) for n in restored["notifies"] if "BlendOut" in n["name"])
            results["montages"].append({"source":before,"target":json.loads(T.describe_montage(target)),"notifies":restored})
        if (len(results["assets"])+len(results["montages"])) % 20 == 0:
            (REPORT_DIR / "adaptation_validation.json").write_text(json.dumps(results,indent=2),encoding="utf-8")
    options = unreal.AssetRegistryDependencyOptions(include_hard_package_references=True,include_soft_package_references=True)
    seeds = [str(d.package_name) for d in registry.get_assets_by_path(ROOT,recursive=True) if str(d.asset_class_path.asset_name) != "ObjectRedirector"]
    pending, seen, edges = list(seeds), set(), {}
    while pending:
        name = pending.pop()
        if name in seen:
            continue
        seen.add(name)
        deps = [str(x) for x in registry.get_dependencies(name,options)]
        edges[name] = deps
        pending.extend(x for x in deps if x.startswith("/Game/") and x not in seen)
    results["output_dependency_packages"] = sorted(seen)
    results["output_dependency_edges"] = edges
    results["passed"] = True
    (REPORT_DIR / "adaptation_validation.json").write_text(json.dumps(results,indent=2),encoding="utf-8")
    unreal.log("GASP_ADAPT_VALIDATED " + str(len(results["assets"])) + " sequences, " + str(len(results["montages"])) + " montages")


main()
