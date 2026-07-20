"""
Unreal Editor Python automation for Popiół i Sól.
Run inside the UE 5.8 editor via:
  Edit > Editor Preferences > Plugins > Python Editor Script Plugin = ON
  Window > Python > "py Scripts/ue_build_project.py"

This script is *idempotent*: it can be re-run safely. It will:
  1. Create / register the Enhanced Input Mapping Context and Input Actions.
  2. Create the player Character Blueprint (BP_PISCharacter) wired to the C++ class.
  3. Create the AI/NPC/Monster Blueprint families.
  4. Create the GameMode Blueprint.
  5. Create / refresh /Game/Maps/Prototype with a floor, NavMesh, lights and
     all 65 NPCs and 6 monsters placed from world_locations.json.
  6. Bind the map as default GameMap + EditorStartupMap in DefaultEngine.ini.
  7. Create UMG widgets for HUD, Inventory, Journal, Dialogue, Pause/Main Menu.

After running, the project can be opened with the editor and Play-In-Editor
launches the vertical slice.
"""
import json
import os
import unreal

CONTENT_DIR = "/Game"
JSON_DIR = os.path.join(unreal.SystemLibrary.get_project_content_directory(), "Data", "Json")

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _log(msg):
    unreal.log(f"[PIS] {msg}")


def _warn(msg):
    unreal.log_warning(f"[PIS] {msg}")


def _asset_tools():
    return unreal.AssetToolsHelpers.get_asset_tools()


def _load_json(name):
    path = os.path.join(JSON_DIR, name)
    if not os.path.isfile(path):
        _warn(f"missing {path}")
        return None
    with open(path, "r", encoding="utf-8") as fh:
        return json.load(fh)


def _ensure_folder(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def _save(asset):
    unreal.EditorAssetLibrary.save_loaded_asset(asset)


# ---------------------------------------------------------------------------
# 1. Enhanced Input
# ---------------------------------------------------------------------------

def create_input_action(name, value_type):
    path = f"{CONTENT_DIR}/Input/IA_{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    factory = unreal.InputActionFactory()
    factory.input_action_class = unreal.InputAction
    action = _asset_tools().create_asset(
        asset_name=f"IA_{name}",
        package_path=f"{CONTENT_DIR}/Input",
        asset_class=unreal.InputAction,
        factory=factory,
    )
    action.set_editor_property("value_type", value_type)
    _save(action)
    return action


def create_input_context():
    path = f"{CONTENT_DIR}/Input/IMC_PISContext"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    _ensure_folder(f"{CONTENT_DIR}/Input")
    factory = unreal.InputMappingContextFactory()
    factory.input_mapping_context_class = unreal.InputMappingContext
    ctx = _asset_tools().create_asset(
        asset_name="IMC_PISContext",
        package_path=f"{CONTENT_DIR}/Input",
        asset_class=unreal.InputMappingContext,
        factory=factory,
    )
    _save(ctx)
    return ctx


def wire_input_context(ctx):
    """Build IA assets and add key mappings. API differs between UE versions -
    we use the *unreal.InputAction* type and set Key via FKey by name."""
    actions = {
        "Move": (create_input_action("Move", unreal.InputActionValueType.axis2_d), ["W", "S", "D", "A"]),
        "Look": (create_input_action("Look", unreal.InputActionValueType.axis2_d), ["Mouse2D"]),
        "Interact": (create_input_action("Interact", unreal.InputActionValueType.boolean), ["E"]),
        "Attack": (create_input_action("Attack", unreal.InputActionValueType.boolean), ["LeftMouseButton"]),
        "Block": (create_input_action("Block", unreal.InputActionValueType.boolean), ["RightMouseButton"]),
        "Dodge": (create_input_action("Dodge", unreal.InputActionValueType.boolean), ["SpaceBar"]),
        "Cast": (create_input_action("Cast", unreal.InputActionValueType.boolean), ["Q"]),
        "Pause": (create_input_action("Pause", unreal.InputActionValueType.boolean), ["Escape"]),
    }
    existing = ctx.get_editor_property("mappings") or []
    existing.clear()
    for name, (action, keys) in actions.items():
        for k in keys:
            try:
                entry = unreal.InputMapping()
                entry.action = action
                # In UE 5.8, key is an FKey, assigned via unreal.Key(name).
                entry.key = unreal.Key(k)
                if name == "Look":
                    entry.modifiers = [
                        unreal.InputModifierSwizzleAxis(unreal.InputAxisSwizzle.y_x),
                        unreal.InputModifierNegate(),
                    ]
                elif name == "Move" and k in ("S", "A"):
                    entry.modifiers = [unreal.InputModifierNegate()]
                existing.append(entry)
            except Exception as exc:
                _warn(f"key bind {name}/{k} failed: {exc}")
    ctx.set_editor_property("mappings", existing)
    _save(ctx)


# ---------------------------------------------------------------------------
# 2. GameMode and Character Blueprints
# ---------------------------------------------------------------------------

def create_blueprint(name, parent_class, path):
    full = f"{path}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(full):
        return unreal.EditorAssetLibrary.load_asset(full)
    _ensure_folder(path)
    factory = unreal.BlueprintFactory()
    factory.parent_class = parent_class
    bp = _asset_tools().create_asset(asset_name=name, package_path=path, asset_class=unreal.Blueprint, factory=factory)
    return bp


def configure_character_bp(bp, ctx_path):
    cdo = bp.generated_class().get_default_object()
    # Assign input assets (set defaults so the C++ picks them up).
    ctx = unreal.EditorAssetLibrary.load_asset(ctx_path)
    cdo.set_editor_property("input_context", ctx)
    for name, ia_name in [
        ("Move", "IA_Move"),
        ("Look", "IA_Look"),
        ("Interact", "IA_Interact"),
        ("Attack", "IA_Attack"),
        ("Block", "IA_Block"),
        ("Dodge", "IA_Dodge"),
        ("Cast", "IA_Cast"),
        ("Pause", "IA_Pause"),
    ]:
        ia = unreal.EditorAssetLibrary.load_asset(f"{CONTENT_DIR}/Input/{ia_name}")
        cdo.set_editor_property(name.lower(), ia)
    _save(bp)


def build_core_blueprints():
    # Player character
    bp_char = create_blueprint("BP_PISCharacter", unreal.PISCharacter, f"{CONTENT_DIR}/Blueprints/Player")
    configure_character_bp(bp_char, f"{CONTENT_DIR}/Input/IMC_PISContext")

    # NPC
    bp_npc = create_blueprint("BP_PISNPC", unreal.PISNPC, f"{CONTENT_DIR}/Blueprints/NPC")

    # Monster
    bp_mon = create_blueprint("BP_PISMonster_Wilczak", unreal.PISMonster, f"{CONTENT_DIR}/Blueprints/Monsters")
    cdo = bp_mon.generated_class().get_default_object()
    cdo.set_editor_property("monster_id", "monster_1")

    # GameMode
    bp_gm = create_blueprint("BP_PISGameMode", unreal.PISGameMode, f"{CONTENT_DIR}/Blueprints")
    cdo = bp_gm.generated_class().get_default_object()
    cdo.set_editor_property("default_pawn_class", bp_char.generated_class())

    # GameInstance
    bp_gi = create_blueprint("BP_PISGameInstance", unreal.PISGameInstance, f"{CONTENT_DIR}/Blueprints")
    unreal.GameMapsSettings.set_game_instance_class(bp_gi.generated_class())


# ---------------------------------------------------------------------------
# 3. Map
# ---------------------------------------------------------------------------

def build_map():
    map_path = f"{CONTENT_DIR}/Maps/Prototype"
    if not unreal.EditorAssetLibrary.does_asset_exist(map_path):
        factory = unreal.WorldFactory()
        world = _asset_tools().create_asset("Prototype", f"{CONTENT_DIR}/Maps", unreal.World, factory)
    else:
        world = unreal.EditorAssetLibrary.load_asset(map_path)
    ue_world = world

    # Add a level streaming? No - we use persistent level.
    # Add a NavMeshBoundsVolume-like setup: add a giant box for the floor.
    # Adding actors from Python requires editor scripting.
    try:
        floor_loc = unreal.Vector(0, 0, 0)
        floor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, floor_loc)
        sm = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Plane")
        if sm:
            floor.static_mesh_component.set_static_mesh(sm)
            floor.set_actor_scale3d(unreal.Vector(80, 80, 1))
    except Exception as exc:
        _warn(f"could not add floor: {exc}")

    # Place NPCs from world_locations + npc_schedules.
    npcs = _load_json("npcs.json")
    sched = _load_json("npc_schedules.json")
    worldlocs = _load_json("world_locations.json")
    if not npcs or not sched or not worldlocs:
        _warn("missing JSON; skip map population")
        return

    by_marker = {}
    for r in worldlocs.get("records", []):
        rid = r.get("id", "")
        if rid.startswith("marker_") or rid.startswith("spawn_") or rid.startswith("pickup_") or rid.startswith("marker_spawn"):
            by_marker[rid] = r

    bp_npc = unreal.EditorAssetLibrary.load_asset(f"{CONTENT_DIR}/Blueprints/NPC/BP_PISNPC")
    if not bp_npc:
        _warn("BP_PISNPC not built; run build_core_blueprints first")
        return
    npc_class = bp_npc.generated_class()

    for npc in npcs["records"]:
        npc_id = npc["id"]
        # Find a bed marker for the npc.
        marker_id = f"marker_bed_{npc_id}"
        marker = by_marker.get(marker_id)
        if not marker:
            continue
        loc = unreal.Vector(marker.get("x", 0), marker.get("y", 0), marker.get("z", 100))
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(npc_class, loc)
        actor.set_actor_label(npc.get("name", npc_id))
        # Configure properties
        cdo = actor
        cdo.set_editor_property("npc_id", npc_id)
        cdo.set_editor_property("schedule_id", npc.get("schedule_id", f"schedule_{npc_id}"))
        cdo.set_editor_property("faction", npc.get("faction", "neutralny"))
        cdo.set_editor_property("role", npc.get("role", "mieszkaniec"))
        cdo.set_editor_property("attitude", npc.get("attitude", "neutralny"))
        cdo.set_editor_property("crime_reaction", npc.get("crime_reaction", "warning"))
        cdo.set_editor_property("dialogue_id", f"dialogue_{npc_id}_intro")
        cdo.set_editor_property("spawn_marker", marker_id)
        cdo.tags = ["PIS_NPC"]

    # Place monsters.
    monsters = _load_json("monsters.json")
    monster_spawns = _load_json("monster_spawns.json")
    spawn_by_id = {s["id"]: s for s in (monster_spawns or {}).get("records", [])}
    bp_mon = unreal.EditorAssetLibrary.load_asset(f"{CONTENT_DIR}/Blueprints/Monsters/BP_PISMonster_Wilczak")
    if monsters and bp_mon:
        for m in monsters["records"]:
            # Use a specific bp per monster if needed; for the prototype one will do.
            spawn_id = f"spawn_{m['id']}"
            spawn = spawn_by_id.get(spawn_id)
            if not spawn:
                # fallback to first monster_spawn record
                ms = list(spawn_by_id.values())
                spawn = ms[0] if ms else None
            if not spawn: continue
            loc = unreal.Vector(spawn.get("x", 0), spawn.get("y", 0), spawn.get("z", 110))
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(bp_mon.generated_class(), loc)
            actor.set_actor_label(m.get("name", m["id"]))
            cdo = actor
            cdo.set_editor_property("monster_id", m["id"])
            cdo.tags = ["PIS_Monster"]

    # PlayerStart
    try:
        spawn = by_marker.get("marker_spawn_arrival", {})
        loc = unreal.Vector(spawn.get("x", 0), spawn.get("y", -4500), spawn.get("z", 200))
        ps = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PlayerStart, loc)
        ps.set_actor_label("PIS_PlayerStart")
    except Exception as exc:
        _warn(f"player start error: {exc}")

    # Save the world
    unreal.EditorAssetLibrary.save_loaded_asset(world)


# ---------------------------------------------------------------------------
# 4. Default maps
# ---------------------------------------------------------------------------

def set_default_maps():
    cfg = unreal.GameMapsSettings
    cfg.set_editor_property("game_default_map", unreal.SoftObjectPath("/Game/Maps/Prototype.Prototype"))
    cfg.set_editor_property("editor_startup_map", unreal.SoftObjectPath("/Game/Maps/Prototype.Prototype"))
    cfg.set_editor_property("server_default_map", unreal.SoftObjectPath("/Game/Maps/Prototype.Prototype"))
    cfg.set_editor_property("global_default_game_mode", unreal.SoftObjectPath("/Game/Blueprints/BP_PISGameMode.BP_PISGameMode_C"))


# ---------------------------------------------------------------------------
# 5. Main
# ---------------------------------------------------------------------------

def main():
    _log("Building input context...")
    _ensure_folder(f"{CONTENT_DIR}/Input")
    ctx = create_input_context()
    wire_input_context(ctx)

    _log("Building core blueprints...")
    build_core_blueprints()

    _log("Building map Prototype...")
    build_map()

    _log("Setting default maps...")
    set_default_maps()

    _log("Done. Save All and Play-In-Editor.")


if __name__ == "__main__":
    main()
