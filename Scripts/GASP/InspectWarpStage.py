import json
from pathlib import Path
import unreal

out = {"montages": [], "root_settings": []}
root = "/Game/Assets/Kongxian/HeroAnimation/GASP"
registry = unreal.AssetRegistryHelpers.get_asset_registry()
registry.search_all_assets(synchronous_search=True)
for data in registry.get_assets_by_path(root + "/Montages", recursive=True):
    if str(data.asset_class_path.asset_name) == "AnimMontage":
        m = data.get_asset()
        out["montages"].append({"montage": json.loads(unreal.GASPRetargetToolsLibrary.describe_montage(m)),
            "notifies": json.loads(unreal.GASPRetargetToolsLibrary.describe_notifies(m))})
rtg = unreal.load_asset(root + "/Rigs/RTG_UEFN_Kongxian_GASP")
controller = unreal.IKRetargeterController.get_controller(rtg)
for i in range(controller.get_num_retarget_ops()):
    op = controller.get_op_controller(i)
    out["root_settings"].append({"name": str(controller.get_op_name(i)),
                                 "type": op.get_class().get_name(),
                                 "settings": str(op.get_settings()) if hasattr(op,"get_settings") else None})
Path("E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget/warp_probe.json").write_text(json.dumps(out, indent=2), encoding="utf-8")
