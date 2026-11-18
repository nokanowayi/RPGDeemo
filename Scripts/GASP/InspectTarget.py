"""Read-only target dependency inventory for a NEW GASP staging run.

Run in the RPGDeemo editor/commandlet. Never saves UE assets. Refuses to
overwrite the existing run's target_dependencies.json; use main(output_file)
to inspect a candidate separately. Added after delivery and verified against
the original manifest using a separate candidate file; never replaces it.
"""
import json
from pathlib import Path
import unreal


SEEDS = (
    "/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady",
    "/Game/Assets/Kongxian/IK_KongXian",
)


def main(output_file=None):
    output = Path(output_file) if output_file else (
        Path(unreal.Paths.project_saved_dir()) / "Codex/GASPRetarget/target_dependencies.json"
    )
    if output.exists():
        raise FileExistsError(
            f"Preserving existing run manifest: {output}. "
            "Call main() with a separate candidate output path to inspect again."
        )

    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    registry.search_all_assets(synchronous_search=True)
    for package in SEEDS:
        if not unreal.load_asset(package):
            raise RuntimeError(f"Required target asset cannot be loaded: {package}")

    options = unreal.AssetRegistryDependencyOptions(
        include_hard_package_references=True,
        include_soft_package_references=True,
        include_searchable_names=False,
        include_hard_management_references=False,
        include_soft_management_references=False,
    )
    pending = list(SEEDS)
    seen = set()
    edges = {}
    content = Path(unreal.Paths.project_content_dir())
    while pending:
        package = pending.pop()
        if package in seen:
            continue
        seen.add(package)
        filename = content / (package.removeprefix("/Game/") + ".uasset")
        if not filename.is_file():
            raise FileNotFoundError(f"Missing /Game dependency: {package} ({filename})")
        dependencies = sorted(str(x) for x in registry.get_dependencies(package, options))
        edges[package] = dependencies
        pending.extend(x for x in dependencies if x.startswith("/Game/") and x not in seen)

    # Keep the same packages/edges schema consumed by PrepareStage.ps1 and
    # DeliverStage.ps1. Engine/plugin references remain in edges but are not
    # copied as project content; matching enabled plugins are a prerequisite.
    report = {"packages": sorted(seen), "edges": {x: edges[x] for x in sorted(edges)}}
    output.parent.mkdir(parents=True, exist_ok=True)
    # Exclusive creation also protects against another inspector writing first.
    with output.open("x", encoding="utf-8") as stream:
        json.dump(report, stream, indent=2)
    print("GASP_TARGET_INVENTORY " + json.dumps({"packages": len(seen), "report": str(output)}))
    return report


if __name__ == "__main__":
    main()
