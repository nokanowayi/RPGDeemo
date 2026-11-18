"""Read-only GASP inventory. Run with UE 5.6 PythonScript commandlet.

The source project and its packages are never saved by this script.
"""
import json
from pathlib import Path
import unreal

OUTPUT = Path("E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget")
ROOT = "/Game/Characters/UEFN_Mannequin"
registry = unreal.AssetRegistryHelpers.get_asset_registry()
registry.search_all_assets(synchronous_search=True)
all_data = registry.get_assets_by_path(ROOT, recursive=True)
selected = [d for d in all_data if any(
    str(d.package_name).startswith(ROOT + "/Animations/" + group + "/")
    for group in ("Crouch", "Jump", "Traversal")) or (
        "/Animations/Idle/M_Neutral_Crouch_Idle_" in str(d.package_name))]
seeds = {str(d.package_name) for d in selected}
seeds.update(str(d.package_name) for d in all_data
             if "/Rigs/IK_UEFN_Mannequin" in str(d.package_name)
             or "/Meshes/" in str(d.package_name))
options = unreal.AssetRegistryDependencyOptions(
    include_hard_package_references=True,
    include_soft_package_references=True,
    include_searchable_names=False,
    include_hard_management_references=False,
    include_soft_management_references=False)
pending = list(seeds)
seen = set()
edges = {}
while pending:
    name = pending.pop()
    if name in seen:
        continue
    seen.add(name)
    deps = [str(x) for x in registry.get_dependencies(name, options)]
    edges[name] = deps
    pending.extend(x for x in deps if x.startswith("/Game/") and x not in seen)

content = Path(unreal.Paths.project_content_dir())
packages = []
for name in sorted(seen):
    filename = content / (name.removeprefix("/Game/") + ".uasset")
    records = registry.get_assets_by_package_name(name)
    packages.append({"package": name, "file": str(filename),
                     "bytes": filename.stat().st_size if filename.exists() else None,
                     "classes": [str(d.asset_class_path.asset_name) for d in records]})
report = {
    "project_dir": unreal.Paths.project_dir(),
    "selected": [{"package": str(d.package_name), "name": str(d.asset_name),
                  "class": str(d.asset_class_path.asset_name)} for d in selected],
    "seeds": sorted(seeds), "dependencies": packages, "edges": edges,
    "mesh_rig_assets": [{"package": str(d.package_name),
                         "class": str(d.asset_class_path.asset_name)} for d in all_data
                        if "/Meshes/" in str(d.package_name) or "/Rigs/" in str(d.package_name)]}
OUTPUT.mkdir(parents=True, exist_ok=True)
(OUTPUT / "source_inventory.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
print("GASP_INVENTORY " + json.dumps({"selected": len(selected), "dependency_packages": len(packages),
     "dependency_MB": round(sum(x["bytes"] or 0 for x in packages)/1048576, 1),
     "report": str(OUTPUT / "source_inventory.json")}))
