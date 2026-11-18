"""Arm before starting PIE. Isolate animation QA from the existing wave-spawn crash."""
from pathlib import Path
import unreal

arm = {}


def on_tick(delta):
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    if not world or "GameModeTestMap" not in world.get_path_name():
        return
    mode = unreal.GameplayStatics.get_game_mode(world)
    pawn = unreal.GameplayStatics.get_player_character(world,0)
    if mode:
        mode.set_actor_tick_enabled(False)
    if not pawn:
        return
    unreal.unregister_slate_post_tick_callback(arm.pop("handle"))
    scope = {"__name__":"__gasp_pie_validation__"}
    filename = Path(unreal.Paths.project_dir()) / "Scripts/GASP/ValidateMontagesPIE.py"
    exec(compile(filename.read_text(encoding="utf-8"),str(filename),"exec"),scope)


arm["handle"] = unreal.register_slate_post_tick_callback(on_tick)
print("GASP_PIE_VALIDATION_ARMED")
