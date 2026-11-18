"""Spawn disposable pose previews in GameModeTestMap PIE, leaving the map asset untouched."""
import json
from pathlib import Path
import unreal


def main():
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    assert world and "GameModeTestMap" in world.get_path_name()
    pawn = unreal.GameplayStatics.get_player_character(world,0)
    pc = unreal.GameplayStatics.get_player_controller(world,0)
    unreal.GameplayStatics.get_game_mode(world).set_actor_tick_enabled(False)
    origin = pawn.get_actor_location()
    forward,right = pawn.get_actor_forward_vector(),pawn.get_actor_right_vector()
    base = origin + forward * 350
    base.z = origin.z - pawn.capsule_component.get_scaled_capsule_half_height()
    mesh = unreal.load_asset("/Game/Assets/Kongxian/SK_Kongxian_UE5_RetargetReady")
    root = "/Game/Assets/Kongxian/HeroAnimation/GASP/Sequences/"
    selections = [
        ("Crouch/Idle/KX_GASP_M_Neutral_Crouch_Idle_Loop",0.0),
        ("Jump/KX_GASP_M_Neutral_Jump_F_Start_Stand_Rfoot",0.4),
        ("Traversal/Climb/KX_GASP_M_Neutral_Traversal_Climb_Start_2_5_stand_F_Lfoot",1.0)]
    actors = {a.get_name():a for a in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.SkeletalMeshActor)}
    cameras = {a.get_name():a for a in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.CameraActor)}
    results=[]
    for i,(path,time) in enumerate(selections):
        location = base + right * ((i-1)*220)
        # Spawn the three disposable actors with UEBridge spawn-actor world:pie.
        actor = actors["SkeletalMeshActor_" + str(i)]
        actor.set_actor_location_and_rotation(location,pawn.get_actor_rotation(),False,True)
        component = actor.skeletal_mesh_component
        component.set_skeletal_mesh_asset(mesh)
        component.set_relative_rotation(pawn.mesh.get_editor_property("relative_rotation"),False,False)
        component.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
        sequence = unreal.load_asset(root+path)
        component.play_animation(sequence,False)
        component.set_position(time,False)
        component.set_play_rate(0.0)
        results.append({"actor":actor.get_name(),"sequence":root+path,"time":time})
    focus = base + unreal.Vector(0,0,130)
    camera_location = base + forward*680 - right*60 + unreal.Vector(0,0,180)
    camera_rotation = unreal.MathLibrary.find_look_at_rotation(camera_location,focus)
    camera = cameras["CameraActor_0"]
    camera.set_actor_location_and_rotation(camera_location,camera_rotation,False,True)
    camera.camera_component.set_field_of_view(60)
    pc.set_view_target_with_blend(camera,0.0)
    report={"map":"/Game/Maps/GameModeTestMap","poses":results,"camera":camera.get_name()}
    out=Path(unreal.Paths.project_saved_dir())/"Codex/GASPRetarget/preview_poses.json"
    out.write_text(json.dumps(report,indent=2),encoding="utf-8")
    print(json.dumps(report))


main()
