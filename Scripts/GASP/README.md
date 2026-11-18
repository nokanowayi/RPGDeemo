# GASP → 空弦离线重定向工具

本目录记录 2026-11-18 的动画资产处理链。已交付 **330 AnimSequence、34 AnimMontage 与 10 个支持包，共 374 个新 package**；目标工程已有的 33 个依赖包未覆盖。完整玩法接入设计与验收见 [方案文档](../../Docs/GASP_Kongxian_LocomotionPlan.md)。

这些脚本用于离线生成和核验资产，不会给角色自动增加蹲下、跳跃或障碍翻越玩法。当前资源已经交付，使用它们无需重新执行本链。

## 运行环境与路径

- UE 5.6.1：`D:/UE_5.6`。
- 目标工程：`E:/UEProject/RPGDeemo/RPGDeemo.uproject`。
- 源工程：`C:/Users/wayi/Documents/Unreal Projects/游戏动画示例/游戏动画示例.uproject`。
- 报告与 Stage：`E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget/`。
- 隔离工程：上述目录下的 `Stage/GASPStage.uproject`。

多数 Python 脚本的报告目录、资产根目录及目标骨架路径是本机固定值，尚未封装成跨机器 CLI。迁移位置时先逐项检查 `REPORT_DIR` / `OUTPUT` / `OUT`、`ROOT` / `TARGET`，以及 PowerShell 的源工程参数，避免把新结果写进旧报告。`Saved` 不应被当成 Git 已提交内容；全新检出可能没有任何 JSON、Stage、工具二进制或验收截图。

修改、重定向和源预览资源清理只允许发生在独立 Stage。Stage 复制源工程 Config 与必要包，既不保存源 GASP 工程，也不在当前 RPGDeemo 工程中执行重定向。不要将 Stage 中的整个 Content 目录迁回项目。

## 脚本职责与执行顺序

| 阶段 | 脚本 / 入口 | 所在工程与输出 |
| --- | --- | --- |
| 1 源清单 | `InspectSource.py` | 源 GASP；只读 AssetRegistry，生成 `source_inventory.json`，包含 Crouch/Jump/Traversal 与 6 条 Crouch Idle 的依赖闭包 |
| 2 目标清单 | `InspectTarget.py` | RPGDeemo；只读目标 Mesh、IK Rig 的 hard+soft `/Game` 包依赖闭包，生成 `target_dependencies.json` |
| 3 隔离复制 | `PrepareStage.ps1` | PowerShell；消费上面两个 JSON，复制依赖及源 Config；Stage 已存在则拒绝覆盖 |
| 4 工具准备 | 下方 BuildPlugin 与 Stage 启用步骤 | 工具只在 Stage 加载，不改目标工程 `.uproject` |
| 5 重定向 | `RetargetStage.py` | Stage；创建 Retargeter，生成 364 动画、整理独立路径、裁剪未接入通知和 UserData、清理源 BlendProfile；结束后退出编辑器 |
| 6 适配核验 | `AdaptValidateStage.py` | Stage；调整 FullBodySlot、恢复 MontageBlendOut、将缺 `attach` 骨的 Warp provider 转为 Static，并比较曲线、同步标记、姿态与 Root Motion |
| 7 新进程重载 | `ExportStageManifest.py` | 重新启动的 Stage；重载全部 364 动画，检查引用、Slot 和通知，输出 `stage_export_manifest.json` 与依赖闭包 |
| 8 限量交付 | `DeliverStage.ps1` | PowerShell；先检查所有冲突和 SHA256，再只复制清单中的新包；已有目标依赖跳过，其余目的包已存在就拒绝 |
| 9 Editor / PIE | 编译支持 BP、`ArmPIEValidation.py`、`ValidateMontagesPIE.py` | 当前 RPGDeemo；`GameModeTestMap` 中抽检实际主 ABP 播放，不写入玩家资产 |
| 10 姿态预览 | `PreviewPosesPIE.py` | 消费 PIE 中临时创建的预览 MeshActor / Camera；不是独立的一键场景生成脚本 |

`ProbeStage.py` 与 `InspectWarpStage.py` 是开发时的只读 API/通知探查入口，不是重新生成的必要步骤。

**补齐目标清单的说明：**本轮原有 `target_dependencies.json` 已有 33 包的执行记录，但此前缺少可随仓库复用的生成脚本。`InspectTarget.py` 是交付收尾新增的入口，已在当前 UE 编辑器只读执行，生成独立的 `target_dependencies_candidate.json`；33 个包的集合与原清单完全一致，**未替换本轮 JSON**。默认输出已经存在时会报错保留原结果。需要检查当前依赖时可在 UE Python 控制台加载模块后，以 `main("独立的候选JSON绝对路径")` 输出对比；不要直接覆盖已交付清单。未来依赖闭包数量可能随目标资源变化，不能硬要求始终为 33。

## 命令行入口

以下是重新生成时的命令模板，不是要求在已交付目录再跑一遍。全新运行必须使用新建的独立工作副本/报告目录，并同步调整硬编码路径。Stage 的源 Rig/预览 Mesh 会被移动，流水线不是可随意重复执行的幂等操作。

UE Python 文件由编辑器/commandlet 执行，不能用系统 Python 直接运行 `import unreal`。先确认源工程启用了 PythonScriptPlugin、EditorScriptingUtilities，以及 Rig/动画、MotionWarping 和依赖要求的引擎插件。`PrepareStage.ps1` 复制源描述文件，不会自动给缺失的插件增加启用项。

```powershell
# 源工程只读清单
& 'D:/UE_5.6/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' `
  'C:/Users/wayi/Documents/Unreal Projects/游戏动画示例/游戏动画示例.uproject' `
  -run=pythonscript '-script=E:/UEProject/RPGDeemo/Scripts/GASP/InspectSource.py' -unattended -nosplash

# 目标工程只读依赖清单；已有输出时主动拒绝覆盖
& 'D:/UE_5.6/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' `
  'E:/UEProject/RPGDeemo/RPGDeemo.uproject' `
  -run=pythonscript '-script=E:/UEProject/RPGDeemo/Scripts/GASP/InspectTarget.py' -unattended -nosplash

& './Scripts/GASP/PrepareStage.ps1'
```

工具编译的已执行命令是：

```powershell
& 'D:/UE_5.6/Engine/Build/BatchFiles/RunUAT.bat' BuildPlugin `
  '-Plugin=E:/UEProject/RPGDeemo/Scripts/GASP/ToolsPlugin/GASPRetargetTools.uplugin' `
  '-Package=E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget/ToolsBuildFinal' `
  -TargetPlatforms=Win64
```

`ToolsBuildFinal.log` 记录 Win64 Development 成功。把打包后的插件放入 **Stage/Plugins/GASPRetargetTools**，并在 **Stage/GASPStage.uproject** 的 Plugins 中添加 `{"Name":"GASPRetargetTools","Enabled":true}`。插件描述文件 `EnabledByDefault=false`、模块类型 `Editor`；只复制目录但未在 Stage 启用时，Python 不会出现 `unreal.GASPRetargetToolsLibrary`。不要复制/启用到当前游戏的 Plugins 或修改当前 `RPGDeemo.uproject`。

按阶段 5、6、7 分别启动 Stage commandlet。每一步检查对应最终 JSON 与日志，确保上一进程结束后再启动下一进程。示例入口：

```powershell
& 'D:/UE_5.6/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' `
  'E:/UEProject/RPGDeemo/Saved/Codex/GASPRetarget/Stage/GASPStage.uproject' `
  -run=pythonscript '-script=E:/UEProject/RPGDeemo/Scripts/GASP/RetargetStage.py' -unattended -nosplash
```

下一进程依次换成 `AdaptValidateStage.py`、`ExportStageManifest.py`。最终导出完成后运行 `DeliverStage.ps1`。后者只为当前 `.uasset` 交付清单实现，不应直接复用于含独立 `.ubulk/.uexp` 等边车数据的其他资产流程。脚本有些错误会写报告再退出，因此退出码不能替代 JSON 的 `complete` / `passed` / `reload_passed` 检查。

当前 UEBridge 执行文件时，采用脚本文本执行包装，避免 `-ScriptPath` 内容解析歧义：

```powershell
& './Saved/Codex/KongxianTail/ue_call.ps1' -Script 'exec(compile(open("E:/UEProject/RPGDeemo/Scripts/GASP/ArmPIEValidation.py",encoding="utf-8").read(),"GASPTask","exec"),{})'
```

`ue_call.ps1` 本身属于本机 `Saved` 辅助工具，不保证全新检出就有；没有时使用 UE Python 控制台执行文件。`ArmPIEValidation.py` 必须在启动 `GameModeTestMap` PIE **之前**注册。它临时关闭 PIE GameMode Tick，以隔离项目原有的刷怪 `FindChecked` 断言，随后播放 5 类 Montage。该停 Tick 行为只服务动画抽检，不能用作正常刷怪的修复方案或玩法验收。

## 最终证据与验证边界

报告均位于 `Saved/Codex/GASPRetarget/`：

| 报告 | 本轮结果 |
| --- | --- |
| `retarget_result.json` | 330 Sequence + 34 Montage，逐项源/目标路径与裁剪记录 |
| `adaptation_validation.json` | `passed=true`；330 序列曲线/同步标记、34 Montage、34 Warp 适配；1,650 姿态和 11,550 有限 Transform |
| `stage_export_manifest.json` | `reload_passed=true`；新进程重载全部 364 动画并导出精确依赖 |
| `delivery_manifest.json` | 374 新包、33 已有依赖保留；含源预览的 1 个材质与 2 个纹理包 |
| `editor_compile.json` | 新 BlendOut 支持 Blueprint 编译成功，0 errors / 0 warnings |
| `pie_validation.json` | `GameModeTestMap`、实际 `KX_ABP_Hero` 的 5 类 Montage 均启动且推进 |
| `preview_poses.json` 与 `poses_preview.png` | 蹲姿、起跳、登台的代表帧；交付图位于 `Docs/Images/GASP_Kongxian_Poses.png` |
| `protection_validation.json` | 1,357 个原有包哈希未变 |
| `ToolsBuildFinal.log` | 仅离线 Editor 工具 BuildPlugin 编译通过 |

本轮没有执行游戏主模块完整 UBT build、玩法 Automation Test、多人联机、任意障碍真实接触与碰撞恢复测试。5 类 Montage 播放与代表帧预览不能证明所有动作衔接或手掌/脚底接触正确。34 个 Static Warp 点是缺辅助骨情况下的初始适配，12 个 Montage 清理了源骨架 BlendProfile，后续接入仍需按方案校准。

保留最终 JSON 和截图作为交付依据；不要把早期失败日志或旧 `retarget_error.txt` 当作最终状态，也不要为了消除历史警告而修改玩家、关卡、尾巴/裙子碰撞或其他已有资源。
