"""Temporary playback smoke test in GameModeTestMap; no asset edits or world refs retained."""
import json
import time
from pathlib import Path
import unreal

OUT = Path(unreal.Paths.project_saved_dir()) / "Codex/GASPRetarget"
report = json.loads((OUT / "retarget_result.json").read_text(encoding="utf-8"))
choices = []
for group in ["/Climb/", "/Hurdle/", "/Mantle/", "/Vault/", "/Catch/Cliff/"]:
    choices.append(next(r["target"] for r in report["assets"] if r["class"] == "AnimMontage" and group in r["target"]))
state = {"index":0,"phase":"start","next":time.monotonic(),"results":[]}


def finish(error=None):
    unreal.unregister_slate_post_tick_callback(state["callback"])
    state.pop("callback",None)
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    if world:
        pawn = unreal.GameplayStatics.get_player_character(world,0)
        if pawn and "mode" in state:
            pawn.mesh.get_anim_instance().montage_stop(0.05)
            pawn.character_movement.set_movement_mode(state["mode"])
    result = {"map":"/Game/Maps/GameModeTestMap","results":state["results"],"passed":error is None and len(state["results"])==5,"error":error,
              "scope":"Montage playback/advancement on actual hero ABP; not obstacle traversal gameplay or network validation"}
    (OUT / "pie_validation.json").write_text(json.dumps(result,indent=2),encoding="utf-8")
    print("GASP_PIE_MONTAGES_DONE " + json.dumps(result))


def tick(delta):
    if time.monotonic() < state["next"]:
        return
    try:
        world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
        assert world and "GameModeTestMap" in world.get_path_name()
        pawn = unreal.GameplayStatics.get_player_character(world,0)
        assert pawn and pawn.mesh.get_anim_instance()
        anim = pawn.mesh.get_anim_instance()
        if "mode" not in state:
            state["mode"] = pawn.character_movement.movement_mode
            pawn.character_movement.stop_movement_immediately()
            pawn.character_movement.disable_movement()
        if state["phase"] == "start":
            montage = unreal.load_asset(choices[state["index"]])
            duration = pawn.play_anim_montage(montage,1.0,"Default")
            assert duration > 0, montage.get_path_name()
            state["results"].append({"montage":choices[state["index"]],"duration":duration})
            state["phase"] = "sample"
            state["next"] = time.monotonic()+0.25
        else:
            current = anim.get_current_active_montage()
            assert current and current.get_path_name().split(".")[0]==choices[state["index"]]
            position = anim.montage_get_position(current)
            assert position > 0
            state["results"][-1].update({"position":position,"anim_instance":anim.get_class().get_path_name(),"passed":True})
            state["index"] += 1
            if state["index"] == len(choices):
                finish()
                return
            state["phase"]="start"
            state["next"]=time.monotonic()+0.1
    except Exception as e:
        finish(str(e))


state["callback"]=unreal.register_slate_post_tick_callback(tick)
print("GASP_PIE_MONTAGES_STARTED")
