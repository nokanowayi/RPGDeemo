# RPGDeemo 安装记录

安装日期：2026-11-17。

- 来源：https://github.com/Monocluar/GASAttachEditor
- 上游分支：main。
- 固定提交：`b19d55682fdd021a39031dc0a372976303d760f3`。
- 许可证：GPL-3.0，原始全文保留于本目录 `LICENSE`。
- 功能：GAS 调试观察面板，可查看角色能力、效果、属性和标签。
- 安装位置：`E:/UEProject/RPGDeemo/Plugins/GASAttachEditor`。
- 项目启用：`RPGDeemo.uproject` 的 Plugins 数组。

## 构建验证

已使用本机 UE 5.6 的 `RunUAT.bat BuildPlugin` 编译 Win64 编辑器模块，退出码为 0。没有修改上游源码。

```powershell
& 'D:/UE_5.6/Engine/Build/BatchFiles/RunUAT.bat' BuildPlugin `
    '-Plugin=E:/UEProject/RPGDeemo/Saved/Codex/PluginDownloads/GASAttachEditor/GASAttachEditor.uplugin' `
    '-Package=E:/UEProject/RPGDeemo/Saved/Codex/PluginBuilds/GASAttachEditor_UE56' `
    -TargetPlatforms=Win64 -NoP4 -UTF8Output
```

DLL 与本机引擎的模块 BuildId 均为 `43139311`。UAT 将安装包描述写为 `EngineVersion=5.6.0`、`Installed=true`，并将旧平台白名单规范为 `PlatformAllowList`；原模块仍为 Editor 类型。

构建有 `STreeView.ItemHeight` 弃用和非推荐 MSVC 工具链版本警告，没有编译错误。仅验证了本机 UE5.6 Win64 构建，其他平台需要重新编译。打包过程未包含 `.git` 与上游的 Intermediate 缓存；LICENSE 与中英文 README 单独补齐。

## 使用

保存自己的编辑器修改并重启 UE，使新原生插件被发现和加载。进入 `GameModeTestMap` 的 PIE 后，在控制台输入：

```text
GASAttachEditorShow
```

也可在 Window 菜单的调试工具分类查找 `Debug Gameplay Ability System`。若角色未列出，先确认正在 PIE 且该角色存在 AbilitySystemComponent。

## 验证边界与风险

- 已验证插件描述、项目启用项、编译结果、引擎 BuildId 及许可证保留。
- 安装时当前编辑器已启动，未关闭或重启，故尚未验证本次进程中的面板加载与 PIE 数据显示。
- 它是 GAS 调试面板；连招数据与派生执行仍由项目战斗逻辑实现。
- `Binaries` 被项目 Git 规则忽略；其他开发者拉取源码后需在自己的 UE 版本下编译，或取得对应编译包。

使用的 UE skill：`unreal-engine-5`、`plugin-module-configuration`、`ubt-integration`。
