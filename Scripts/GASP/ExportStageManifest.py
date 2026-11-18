"""Fresh-process reload checks and exact dependency manifest for delivery."""
import hashlib
import json
from pathlib import Path
import unreal

OUT = Path("E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget")
ROOT = "/Game/Assets/Kongxian/HeroAnimation/GASP"
registry = unreal.AssetRegistryHelpers.get_asset_registry()
registry.search_all_assets(synchronous_search=True)
report = json.loads((OUT / "retarget_result.json").read_text(encoding="utf-8"))
adapt = json.loads((OUT / "adaptation_validation.json").read_text(encoding="utf-8"))
assert adapt["passed"]
reloaded = []
for row in report["assets"]:
    asset = unreal.load_asset(row["target"])
    assert asset and asset.get_editor_property("skeleton").get_path_name().startswith("/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady_Skeleton.")
    notify_records = []
    for event in unreal.AnimationLibrary.get_animation_notify_events(asset):
        notify = event.get_editor_property("notify") or event.get_editor_property("notify_state_class")
        name = str(event.get_editor_property("notify_name"))
        if "BlendOut" in name:
            assert notify and "MontageBlendOut" in notify.get_class().get_name(), row["target"]
        if notify:
            cls = notify.get_class().get_path_name()
            assert cls.startswith("/Script/Engine.") or cls.startswith("/Script/MotionWarping.") or cls.startswith("/Game/Blueprints/AnimNotifies/BP_NotifyState_MontageBlendOut."), (row["target"],cls)
            if isinstance(notify,unreal.AnimNotifyState_MotionWarping):
                mod = notify.get_editor_property("root_motion_modifier")
                assert mod.get_editor_property("warp_point_anim_provider") != unreal.WarpPointAnimProvider.BONE
            notify_records.append({"name":name,"class":cls,"time":unreal.AnimationLibrary.get_anim_notify_event_trigger_time(event),
                                   "duration":unreal.AnimationLibrary.get_anim_notify_event_duration(event)})
    if isinstance(asset,unreal.AnimMontage):
        desc = json.loads(unreal.GASPRetargetToolsLibrary.describe_montage(asset))
        assert all(s["name"] == "FullBodySlot" for s in desc["slots"])
        assert any("BlendOut" in x["name"] for x in notify_records)
    reloaded.append({"asset":row["target"],"notifies":notify_records})

seeds = [r["target"] for r in report["assets"]] + [report["retargeter"]]
options = unreal.AssetRegistryDependencyOptions(include_hard_package_references=True,include_soft_package_references=True)
pending, seen, edges = list(seeds), set(), {}
while pending:
    name = pending.pop()
    if name in seen:
        continue
    seen.add(name)
    dependencies = [str(d) for d in registry.get_dependencies(name,options)]
    edges[name] = dependencies
    pending.extend(d for d in dependencies if d.startswith("/Game/") and d not in seen)

content = Path(unreal.Paths.project_content_dir())
files = []
for package in sorted(seen):
    filename = content / (package.removeprefix("/Game/") + ".uasset")
    assert filename.is_file(), package
    data = registry.get_assets_by_package_name(package)
    assert not any(str(d.asset_class_path.asset_name) == "ObjectRedirector" for d in data), package
    files.append({"package":package,"file":str(filename),"sha256":hashlib.sha256(filename.read_bytes()).hexdigest(),
                  "bytes":filename.stat().st_size,"classes":[str(d.asset_class_path.asset_name) for d in data]})
manifest = {"sequences":330,"montages":34,"reloaded":reloaded,"files":files,"dependency_edges":edges,
            "reload_passed":True,"existing_target_prefix":"/Game/Assets/Kongxian/"}
(OUT / "stage_export_manifest.json").write_text(json.dumps(manifest,indent=2),encoding="utf-8")
unreal.log("GASP_EXPORT_MANIFEST " + json.dumps({"packages":len(files),"MB":sum(f["bytes"] for f in files)/1048576}))
