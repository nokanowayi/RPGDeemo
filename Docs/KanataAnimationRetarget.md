# 空弦 Kanata 动画与 Montage

2026-11-17：已将 Aurora 的 Ability E、Ability R、普通攻击 A/B/C，以及用户指定的 Fast D，直接重定向到当前玩家空弦骨架。生成 6 个 AnimSequence、6 个 AnimMontage 和 1 个独立 IK Retargeter。

## 项目与骨架

- 项目：`E:/UEProject/RPGDeemo/RPGDeemo.uproject`，UE 5.6。
- 玩家：`/Game/PlayerCharacter/BP_HeroCharacter`。
- 来源网格：`/Game/ParagonAurora/Characters/Heroes/Aurora/Meshes/Aurora`。
- 目标网格：`/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady`。
- 目标骨架：`/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady_Skeleton`。
- 主动画蓝图：`/Game/Assets/Kongxian/HeroAnimation/KX_ABP_Hero`。

这里针对的是游戏玩家使用的 RetargetReady 版本；MainMenuMap 展示用的 `KongxianAI/Kongxian_UE5_final` 是另一套骨架。

## 问题、优先级与处理

| 优先级 | 问题 | 处理 |
| --- | --- | --- |
| P1 | Aurora 原骨架不能直接用于空弦 Montage | 使用 Aurora → 空弦的独立 IK Retargeter 烘焙动画 |
| P1 | 当前主 ABP 输出经过 FullBodySlot，默认 Slot 不保证可见 | 六个 Montage 都设为 `DefaultGroup.FullBodySlot` |
| P2 | 没有普通 `Primary_Attack_D` | 按用户选择使用 `Primary_Attack_Fast_D` |
| P2 | 旧库是 Aurora → Hero → 空弦的间接转换 | 本次从 Aurora 原始动画直接转换，并独立保存为 Kanata 资源 |

Retargeter 使用现有 `IK_Aurora` 和 `IK_KongXian`，显式指定双方预览网格，按相同链名映射，并对目标执行 Chain To Chain 姿态对齐。没有把旧 Hero Retargeter 的源骨架直接当作 Aurora。

## 生成资源

资源根目录：`/Game/Assets/Kongxian/HeroAnimation/Kanata`。

以下名称相对于根目录：

| Aurora 原始动画 | 新序列：Sequences/ | 新 Montage：Montages/ | 时长 |
| --- | --- | --- | ---: |
| Ability_E | A_KX_Kanata_Ability_E | AM_KX_Kanata_Ability_E | 2.4 秒 |
| Ability_R | A_KX_Kanata_Ability_R | AM_KX_Kanata_Ability_R | 2.0 秒 |
| Primary_Attack_A | A_KX_Kanata_Attack_A | AM_KX_Kanata_Attack_A | 1.0 秒 |
| Primary_Attack_B | A_KX_Kanata_Attack_B | AM_KX_Kanata_Attack_B | 1.0 秒 |
| Primary_Attack_C | A_KX_Kanata_Attack_C | AM_KX_Kanata_Attack_C | 1.0 秒 |
| Primary_Attack_Fast_D | A_KX_Kanata_Attack_D_Fast | AM_KX_Kanata_Attack_D_Fast | 约 1.767 秒 |

重定向器：`Rigs/RTG_Aurora_Kongxian_Kanata`。

六份均采用非 Additive 的完整动画。每个 Montage 包含一个对应的新序列和 `Default` Section，播放速率为 1；没有拼接成一个总连段 Montage。

## 使用与风险

在 Kanata 攻击 GA 中将 Montage 引用设为表中的 `AM_KX_Kanata_*`。这些资产提供动作播放；伤害检测、派生窗口、连招事件和资源结算需要由招式逻辑或相应 Notify 配置。新 Montage 没有自动套用旧斧头的伤害通知。

Ability_R 保留原动画的 `Enable Root Motion=true`；其他五份为 false。关闭提取后逐帧读取，R 的 Root Z 范围由原 Aurora 的约 0–76.59 cm 缩放为目标约 0–62.08 cm；起止位置都为零，属于中间抬高再返回。它并非向前突进动画，实际角色竖直移动仍受 CharacterMovement 模式与 Root Motion 配置影响。Root Lock 和 Force Root Lock 与来源一致。

握刀 Socket、手腕方向、两手贴合、脚底接触和招式衔接仍需要结合太刀模型逐招调整。本次没有更改 GA 的能力引用，也没有修改玩家、武器或地图配置。

## 验证结果与后续验收

- Editor：13 个新资产创建/保存成功；Montage 的 Skeleton、序列引用、时长、Section、FullBodySlot 均已核对。Attack A 在 Montage 编辑器中使用目标空弦模型预览。
- 数据检查：6 个序列合计采样 281 帧，每帧 558 根骨；无非有限变换，Root 缩放约为 1，无百倍缩放问题。源/目标时长与 Root Motion 开关、Root Lock 设置一致。
- PIE：从 `GameModeTestMap` 启动，在实际 `BP_HeroCharacter / KX_ABP_Hero` 上逐个启动六个 Montage，每个观察约 0.3 秒，确认激活实例与播放位置推进。测试结束已退出 PIE。
- 本轮 PIE 验证不覆盖整招播放、伤害通知、连招衔接、结束回调或联机效果；接入 GA 后应在同一地图验证完整玩法。
- UBT：本轮没有 C++、Build.cs 或反射声明修改，未执行 UBT。
- Automation Test：未新增持久测试；本轮使用 Editor 数据检查与 PIE 播放冒烟检查。

执行脚本和机器可读报告位于 `E:/UEProject/RPGDeemo/Saved/Codex/KanataRetarget/`，其中 `result.json` 记录资源映射、骨架、根骨检查、PIE 采样及原资源磁盘哈希。Saved 目录通常不纳入 Git。

使用的 UE skill：`unreal-engine-5`，动画主题 `anim-montage-cpp`；按实际 IKRig 和 AnimMontage 工厂 API 执行 Editor 资产工作流。
