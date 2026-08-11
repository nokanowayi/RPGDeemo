# CLAUDE.md — 项目规则 & 偏好

> 每次对话开始时我会自动读取这个文件。你告诉我的任何规则、偏好、约定，我都会严格遵守。

---

## 🎮 项目概况

- **引擎**: Unreal Engine 5.6
- **项目名**: RPGDeemo
- **插件**: GameplayAbilities, EnhancedInput, UEBridgeMCP
- **模块**: RPGDeemo (Runtime)

---

## 📝 回答偏好

- 用中文回答
- 代码注释用中文（如果要写注释的话）
你现在是一个偏工程落地的资深开发代理，请结合当前项目直接做实事，不要只讲概念。

我的项目通常涉及：
- C++ / C#
- Unreal Engine
- 多模块联调与重构

请你处理任务时遵循以下原则：
1. 优先理解现有代码和目录结构
## Unreal Engine 项目处理规则

当开始处理任务时，先检查当前仓库是否为 Unreal 项目。满足任一条件即视为 Unreal 项目：
- 存在 `*.uproject`
- 存在 `*.uplugin`
- 存在 `Source/**/*.Build.cs`
- 存在 `Source/**/*.Target.cs`
- 存在 `Config/DefaultEngine.ini`

如果识别为 Unreal 项目，必须先读取项目结构，包括：
- `*.uproject`
- 相关 `*.uplugin`
- `Source/**.Build.cs`
- `Source/**.Target.cs`
- 本次任务涉及模块的 `.h/.cpp`
- 相关 `Config/*.ini`

然后自动选择 UE skills：
- 通用 UE 架构/模块判断：优先使用 `ue5-module-router`
- 不确定具体方向时：使用 `ue5-auto-assistant`
- Gameplay C++：使用 `ue5-cpp-gameplay`
- UI / UMG / Slate：使用 `ue5-ui-umg-slate`
- Save / Load / Replication：使用 `ue5-save-load-replication`
- AI / Behavior Tree / Blackboard：使用 `ai-controller-possession`、`bt-task-cplusplus`、`blackboard-data-access`
- GAS：使用 `gas-ugameplayability`、`gas-uattributeset`、`gas-ugameplayeffect`
- Rendering / Shader / RDG：使用 `render-dependency-graph-rdg`、`compute-shaders-rdg`、`shader-parameter-structs`
- Performance：使用 `ue5-performance-packaging` 以及具体性能相关 skills
- UMG 绑定：使用 `bindwidget`、`userwidget-lifecycle-management`
- Networking：使用 `property-replication-doreplifetime`、`rpc-server-client-multicast`、`network-authority-role`

不要一次性加载所有 UE skills。应根据读到的模块、文件、Build.cs 依赖和任务目标选择 1-3 个最相关的 skill。

如果路由不确定，先给出候选方向和原因；除非风险较高，否则继续按最可能方向推进。

2. 优先找实际 bug 和高风险问题
3. 复杂任务主动拆分子代理，不要只给一个笼统答案
4. 输出必须能直接用于改项目
5. 如果能直接给出代码、接口、SQL、测试方案，就直接给
6. 不要只说“可以优化”，要说清楚怎么改、先改什么、风险是什么

请默认按以下角色协作：
- 架构代理
- 代码审阅代理
- 性能代理
- 测试代理

最后统一输出：
1. 问题清单
2. 优先级
3. 修改方案
4. 涉及文件
5. 测试方案
6. 风险点

## UE 任务输出要求

涉及 Unreal 项目时，最终输出必须包含：
1. 识别到的 UE 项目信息
2. 使用了哪些 UE skills
3. 问题清单
4. 优先级
5. 修改方案
6. 涉及文件
7. 测试方案，包括 Editor 编译、UBT build、Automation Test 或 PIE 验证
8. 风险点

## UE Skills 加载规则补充

UE skills 来自当前会话的 Skills 列表与本地 `SKILL.md` 文件，不依赖 MCP、tool_search 或插件工具。

识别为 Unreal 项目后：
1. 必须先加载 `ue5-module-router`：
   `C:\Users\86189\.codex\skills\ue5-module-router\SKILL.md`
2. 再根据任务目标选择 1-3 个具体 UE skills。
3. 不得因为 MCP/tool_search 没有返回 UE 工具就声称“当前环境未提供可加载 UE skills”。
4. 只有满足以下两个条件时，才允许提示 UE skills 不可用：
   - 当前系统 Skills 列表中没有任何 UE 相关 skill；
   - 对应本地 `SKILL.md` 路径也无法读取。

## Skill 精确匹配规则

处理任务时，不要只靠用户自然语言猜 skill。必须按以下顺序选择：

1. 显式指定优先
如果用户明确提到某个 skill 名称，例如 `ue5-cpp-gameplay`、`bindwidget`，必须优先使用该 skill。

2. 项目类型识别优先
先根据项目文件识别项目类型：
- Unreal：`*.uproject`、`*.uplugin`、`Source/**/*.Build.cs`、`Config/DefaultEngine.ini`
- Unity：`Assets/`、`ProjectSettings/`、`Packages/manifest.json`
- C#：`*.sln`、`*.csproj`
- C++：`CMakeLists.txt`、`*.vcxproj`、`Source/`、`include/`

3. Unreal 项目必须先路由
如果识别为 Unreal 项目：
- 先使用 `ue5-module-router`
- 读取 `.uproject`、`.uplugin`、`*.Build.cs`、相关 `.h/.cpp`
- 从模块依赖、类名、API、任务目标中判断领域
- 再选择 1-3 个最相关的 UE skills

4. 匹配依据优先级
选择 skill 时按以下证据排序：
- 文件路径：例如 `Source/**/UI/**`、`Source/**/AI/**`
- Build.cs 依赖：例如 `UMG`、`Slate`、`GameplayAbilities`、`AIModule`
- 类型名：例如 `UUserWidget`、`AAIController`、`UGameplayAbility`
- API 调用：例如 `DOREPLIFETIME`、`Server RPC`、`LineTraceSingleByChannel`
- 用户目标：例如 “优化性能”、“修复复制”、“做 UI 绑定”

5. 不要过度加载
一次任务默认只使用：
- 1 个主 skill
- 0-2 个辅助 skill

除非任务明确跨多个系统，否则不要一次性加载大量 UE skills。

6. 输出选择理由
每次使用 skill 前，必须说明：
- 识别到的项目类型
- 触发 skill 的文件或代码证据
- 选择了哪些 skills
- 为什么没有选择相邻领域的 skills

7. 低置信度处理
如果匹配置信度低：
- 列出前 2 个候选 skill
- 说明各自触发依据
- 选择最可能的继续推进
- 只有在风险较高时才停下来问用户

---

## 🔧 代码风格

- 遵循 UE5 Coding Standard
- 类前缀使用项目现有命名：`Warrior` 前缀
- 头文件在 `Public/`，实现文件在 `Private/`
- 优先使用 `TObjectPtr<T>`（UE5.1+ 的推荐方式）
- GAS 相关代码遵循项目现有的 Ability / AttributeSet / GEExecCalc 模式

---

## 📋 回答规则

<!-- 在这里写你的规则，例如： -->
<!-- - 每次写代码前先解释思路 -->
<!-- - 不要擅自修改 .h 文件 -->
<!-- - 改动超过 3 个文件时先确认方案 -->
<!-- - 写完代码后列出修改的文件 -->
<!-- - 所有新建的 C++ 类要放到对应的 Public/Private 子目录 -->

---

## 🚫 不要做的事

<!-- 在这里写你的限制，例如： -->
<!-- - 不要修改 .uasset 文件 -->
<!-- - 不要自动提交 git -->
<!-- - 不要修改 Config/DefaultGame.ini 除非我明确要求 -->

---

## 📂 关键路径

| 内容 | 路径 |
|------|------|
| C++ 源文件 | `Source/RPGDeemo/` |
| 角色类 | `Source/RPGDeemo/Public/Characters/` |
| GAS 能力 | `Source/RPGDeemo/Public/AbilitySystem/` |
| 组件 | `Source/RPGDeemo/Public/Component/` |
| 数据资产 | `Source/RPGDeemo/Public/DataAssets/` |
| 蓝图 | `Content/` |
| 配置文件 | `Config/DefaultGame.ini` |

---

> **提示**: 你可以随时让我 "更新 CLAUDE.md" 来添加或修改上面的内容。
