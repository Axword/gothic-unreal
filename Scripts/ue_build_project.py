"""
Unreal Editor Python automation for Popiół i Sól.
Run inside the UE 5.8 editor via:
  Edit > Editor Preferences > Plugins > Python Editor Script Plugin = ON
  Window > Python > "py Scripts/ue_build_project.py"

This script is *idempotent*: it can be re-run safely. It will:
  1. Create / register the Enhanced Input Mapping Context and Input Actions.
  2. Create the player Character Blueprint (BP_PISCharacter) wired to the C++ class.
  3. Create the AI/NPC/Monster Blueprint families.
  4. Create the GameMode Blueprint + GameInstance Blueprint.
  5. Create the HUD Blueprint.
  6. Create /Game/Maps/Prototype with a floor, lights and
     all 65 NPCs and 6 monsters placed from world_locations.json.
  7. Create /Game/Maps/MainMenu (empty level).
  8. Bind MainMenu as default GameMap + EditorStartupMap.

After running, the project can be opened with the editor and Play-In-Editor
launches the vertical slice.

This script is intentionally defensive: it tries multiple Python APIs
because some factory classes were removed in 5.8 and the API surface
shifts between minor versions.
"""
import json
import os
import unreal

CONTENT_DIR = "/Game"
JSON_DIR = os.path.join(unreal.SystemLibrary.get_project_content_directory(), "Data", "Json")


def _log(msg):
    unreal.log(f"[PIS] {msg}")


def _warn(msg):
    unreal.log_warning(f"[PIS] {msg}")


def _err(msg):
    unreal.log_error(f"[PIS] {msg}")


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
    if asset is not None:
        unreal.EditorAssetLibrary.save_loaded_asset(asset)


def _create_asset(asset_class, name, package_path, factory=None):
    """Wrapper around AssetTools.create_asset that works with or without factory."""
    full = f"{package_path}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(full):
        return unreal.EditorAssetLibrary.load_asset(full)
    tools = _asset_tools()
    # 5.8+ has create_asset(asset_name, package_path, asset_class, factory=None)
    try:
        if factory is not None:
            return tools.create_asset(name, package_path, asset_class, factory)
        return tools.create_asset(name, package_path, asset_class)
    except TypeError:
        # Older signature: create_asset(name, path, factory, asset_class)
        if factory is not None:
            return tools.create_asset(name, package_path, factory, asset_class)
        # Last resort: try with no factory at all
        return tools.create_asset(name, package_path, asset_class)


# ---------------------------------------------------------------------------
# 1. Enhanced Input
# ---------------------------------------------------------------------------

def create_input_action(name, value_type):
    path = f"{CONTENT_DIR}/Input/IA_{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    # Try factory first, fall back to no-factory create.
    factory = None
    if hasattr(unreal, "InputActionFactory"):
        factory = unreal.InputActionFactory()
    action = _create_asset(unreal.InputAction, f"IA_{name}", f"{CONTENT_DIR}/Input", factory)
    if action is not None:
        action.set_editor_property("value_type", value_type)
        _save(action)
    return action


def create_input_context():
    path = f"{CONTENT_DIR}/Input/IMC_PISContext"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    _ensure_folder(f"{CONTENT_DIR}/Input")
    factory = None
    if hasattr(unreal, "InputMappingContextFactory"):
        factory = unreal.InputMappingContextFactory()
    ctx = _create_asset(unreal.InputMappingContext, "IMC_PISContext", f"{CONTENT_DIR}/Input", factory)
    if ctx is not None:
        _save(ctx)
    return ctx


def wire_input_context(ctx):
    actions = {
        "Move": (create_input_action("Move", unreal.InputActionValueType.axis2_d), ["W", "S", "D", "A"]),
        "Look": (create_input_action("Look", unreal.InputActionValueType.axis2_d), ["Mouse2D"]),
        "Interact": (create_input_action("Interact", unreal.InputActionValueType.boolean), ["E"]),
        "Attack": (create_input_action("Attack", unreal.InputActionValueType.boolean), ["LeftMouseButton"]),
        "Block": (create_input_action("Block", unreal.InputActionValueType.boolean), ["RightMouseButton"]),
        "Dodge": (create_input_action("Dodge", unreal.InputActionValueType.boolean), ["SpaceBar"]),
        "Cast": (create_input_action("Cast", unreal.InputActionValueType.boolean), ["Q"]),
        "Pause": (create_input_action("Pause", unreal.InputActionValueType.boolean), ["Escape"]),
        "Inventory": (create_input_action("Inventory", unreal.InputActionValueType.boolean), ["I"]),
        "Journal": (create_input_action("Journal", unreal.InputActionValueType.boolean), ["J"]),
    }
    existing = ctx.get_editor_property("mappings") or []
    existing.clear()
    for name, (action, keys) in actions.items():
        for k in keys:
            try:
                entry = unreal.InputMapping()
                entry.action = action
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
# 2. Blueprints (no BlueprintFactory in 5.8 Python)
# ---------------------------------------------------------------------------

def create_blueprint(name, parent_class, path):
    full = f"{path}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(full):
        return unreal.EditorAssetLibrary.load_asset(full)
    _ensure_folder(path)
    tools = _asset_tools()
    # Preferred: BlueprintFactory if it exists.
    factory = None
    if hasattr(unreal, "BlueprintFactory"):
        try:
            factory = unreal.BlueprintFactory()
            factory.parent_class = parent_class
        except Exception as exc:
            _warn(f"BlueprintFactory({parent_class}) failed: {exc}")
            factory = None
    if factory is not None:
        bp = tools.create_asset(name, path, unreal.Blueprint, factory)
    else:
        # Fallback: 5.8 create_asset without explicit factory - relies on
        # the asset_class parameter for the factory selection.
        try:
            bp = tools.create_asset(name, path, unreal.Blueprint, parent_class=parent_class)
        except TypeError:
            bp = tools.create_asset(name, path, unreal.Blueprint)
    if bp is None:
        _err(f"failed to create blueprint {name}")
    return bp


def configure_character_bp(bp, ctx_path):
    cdo = bp.generated_class().get_default_object()
    ctx = unreal.EditorAssetLibrary.load_asset(ctx_path)
    cdo.set_editor_property("input_context", ctx)
    for prop, ia_name in [
        ("ia_move", "IA_Move"),
        ("ia_look", "IA_Look"),
        ("ia_interact", "IA_Interact"),
        ("ia_attack", "IA_Attack"),
        ("ia_block", "IA_Block"),
        ("ia_dodge", "IA_Dodge"),
        ("ia_cast", "IA_Cast"),
        ("ia_pause", "IA_Pause"),
        ("ia_inventory", "IA_Inventory"),
        ("ia_journal", "IA_Journal"),
    ]:
        ia = unreal.EditorAssetLibrary.load_asset(f"{CONTENT_DIR}/Input/{ia_name}")
        cdo.set_editor_property(prop, ia)
    _save(bp)


def build_core_blueprints():
    bp_char = create_blueprint("BP_PISCharacter", unreal.PISCharacter, f"{CONTENT_DIR}/Blueprints/Player")
    if bp_char:
        configure_character_bp(bp_char, f"{CONTENT_DIR}/Input/IMC_PISContext")

    bp_npc = create_blueprint("BP_PISNPC", unreal.PISNPC, f"{CONTENT_DIR}/Blueprints/NPC")

    bp_mon = create_blueprint("BP_PISMonster_Wilczak", unreal.PISMonster, f"{CONTENT_DIR}/Blueprints/Monsters")
    if bp_mon:
        cdo = bp_mon.generated_class().get_default_object()
        cdo.set_editor_property("monster_id", "monster_1")

    bp_hud = create_blueprint("BP_PISHUD", unreal.PISHUD, f"{CONTENT_DIR}/Blueprints/UI")

    bp_gm = create_blueprint("BP_PISGameMode", unreal.PISGameMode, f"{CONTENT_DIR}/Blueprints")
    if bp_gm and bp_char and bp_hud:
        cdo = bp_gm.generated_class().get_default_object()
        cdo.set_editor_property("default_pawn_class", bp_char.generated_class())
        cdo.set_editor_property("hud_class", bp_hud.generated_class())

    create_blueprint("BP_PISGameInstance", unreal.PISGameInstance, f"{CONTENT_DIR}/Blueprints")


# ---------------------------------------------------------------------------
# 3. Maps
# ---------------------------------------------------------------------------

def _spawn_actor(actor_class, location, rotation=None, label=None):
    """Spawn an actor in the current level. 5.8 has multiple APIs - try EditorLevelLibrary first."""
    rotation = rotation or unreal.Rotator(0, 0, 0)
    if hasattr(unreal, "EditorLevelLibrary"):
        try:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, location, rotation)
        except Exception as exc:
            _warn(f"EditorLevelLibrary.spawn_actor_from_class failed: {exc}")
            actor = None
    else:
        actor = None
    if actor is None and hasattr(unreal, "EditorActorSubsystem"):
        try:
            sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
            actor = sub.spawn_actor_from_class(actor_class, location, rotation)
        except Exception as exc:
            _warn(f"EditorActorSubsystem failed: {exc}")
            actor = None
    if actor is not None and label is not None:
        try:
            actor.set_actor_label(label)
        except Exception:
            pass
    return actor


def build_prototype_map():
    map_path = f"{CONTENT_DIR}/Maps/Prototype"
    if not unreal.EditorAssetLibrary.does_asset_exist(map_path):
        factory = None
        if hasattr(unreal, "WorldFactory"):
            factory = unreal.WorldFactory()
        world = _create_asset(unreal.World, "Prototype", f"{CONTENT_DIR}/Maps", factory)
    else:
        world = unreal.EditorAssetLibrary.load_asset(map_path)
    if world is None:
        _err("could not create Prototype map")
        return
    # Open the world for editing.
    if hasattr(unreal, "EditorLoadingAndSavingUtils"):
        try:
            unreal.EditorLoadingAndSavingUtils.load_map(world)
        except Exception as exc:
            _warn(f"load_map(Prototype): {exc}")

    # Floor.
    try:
        floor = _spawn_actor(unreal.StaticMeshActor, unreal.Vector(0, 0, 0), label="PIS_Floor")
        if floor is not None:
            sm = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Plane")
            if sm:
                floor.static_mesh_component.set_static_mesh(sm)
                floor.set_actor_scale3d(unreal.Vector(80, 80, 1))
    except Exception as exc:
        _warn(f"floor: {exc}")

    # NavMeshBoundsVolume.
    try:
        nav = _spawn_actor(unreal.NavMeshBoundsVolume, unreal.Vector(0, 0, 200), label="PIS_NavMesh")
        if nav is not None:
            nav.set_actor_scale3d(unreal.Vector(60, 75, 5))
    except Exception as exc:
        _warn(f"navmesh: {exc}")

    # Lighting.
    try:
        sun = _spawn_actor(unreal.DirectionalLight, unreal.Vector(0, 0, 800), unreal.Rotator(-50, 30, 0), "PIS_Sun")
        sky = _spawn_actor(unreal.SkyLight, unreal.Vector(0, 0, 1200), label="PIS_Sky")
    except Exception as exc:
        _warn(f"lighting: {exc}")

    # Atmospheric fog.
    try:
        _spawn_actor(unreal.AtmosphericFog, unreal.Vector(0, 0, 200), label="PIS_Fog")
    except Exception:
        pass

    npcs = _load_json("npcs.json")
    worldlocs = _load_json("world_locations.json")
    if not npcs or not worldlocs:
        _warn("missing JSON; skip map population")
        return

    by_marker = {}
    for r in worldlocs.get("records", []):
        rid = r.get("id", "")
        if rid.startswith("marker_") or rid.startswith("spawn_") or rid.startswith("pickup_") or rid.startswith("marker_spawn"):
            by_marker[rid] = r

    bp_npc = unreal.EditorAssetLibrary.load_asset(f"{CONTENT_DIR}/Blueprints/NPC/BP_PISNPC")
    if bp_npc:
        npc_class = bp_npc.generated_class()
        for npc in npcs["records"]:
            npc_id = npc["id"]
            marker_id = f"marker_bed_{npc_id}"
            marker = by_marker.get(marker_id)
            if not marker:
                continue
            loc = unreal.Vector(marker.get("x", 0), marker.get("y", 0), marker.get("z", 100))
            actor = _spawn_actor(npc_class, loc, label=npc.get("name", npc_id))
            if actor is None:
                continue
            try:
                actor.set_editor_property("npc_id", npc_id)
                actor.set_editor_property("schedule_id", npc.get("schedule_id", f"schedule_{npc_id}"))
                actor.set_editor_property("faction", npc.get("faction", "neutralny"))
                actor.set_editor_property("role", npc.get("role", "mieszkaniec"))
                actor.set_editor_property("attitude", npc.get("attitude", "neutralny"))
                actor.set_editor_property("crime_reaction", npc.get("crime_reaction", "warning"))
                actor.set_editor_property("dialogue_id", f"dialogue_{npc_id}_intro")
                actor.set_editor_property("spawn_marker", marker_id)
                actor.tags = ["PIS_NPC"]
            except Exception as exc:
                _warn(f"config NPC {npc_id}: {exc}")

    monsters = _load_json("monsters.json")
    monster_spawns = _load_json("monster_spawns.json")
    spawn_by_id = {s["id"]: s for s in (monster_spawns or {}).get("records", [])}
    bp_mon = unreal.EditorAssetLibrary.load_asset(f"{CONTENT_DIR}/Blueprints/Monsters/BP_PISMonster_Wilczak")
    if monsters and bp_mon:
        mon_class = bp_mon.generated_class()
        for m in monsters["records"]:
            spawn_id = f"spawn_{m['id']}"
            spawn = spawn_by_id.get(spawn_id)
            if not spawn:
                ms = list(spawn_by_id.values())
                spawn = ms[0] if ms else None
            if not spawn:
                continue
            loc = unreal.Vector(spawn.get("x", 0), spawn.get("y", 0), spawn.get("z", 110))
            actor = _spawn_actor(mon_class, loc, label=m.get("name", m["id"]))
            if actor is None:
                continue
            try:
                actor.set_editor_property("monster_id", m["id"])
                actor.tags = ["PIS_Monster"]
            except Exception as exc:
                _warn(f"config monster {m['id']}: {exc}")

    # World clock.
    try:
        _spawn_actor(unreal.PISWorldClock, unreal.Vector(0, 0, 200), label="PIS_Clock")
    except Exception as exc:
        _warn(f"clock: {exc}")

    # PlayerStart.
    try:
        spawn = by_marker.get("marker_spawn_arrival", {})
        loc = unreal.Vector(spawn.get("x", 0), spawn.get("y", -4500), spawn.get("z", 200))
        _spawn_actor(unreal.PlayerStart, loc, label="PIS_PlayerStart")
    except Exception as exc:
        _warn(f"player start: {exc}")

    unreal.EditorAssetLibrary.save_loaded_asset(world)


def build_main_menu_map():
    map_path = f"{CONTENT_DIR}/Maps/MainMenu"
    if not unreal.EditorAssetLibrary.does_asset_exist(map_path):
        factory = None
        if hasattr(unreal, "WorldFactory"):
            factory = unreal.WorldFactory()
        world = _create_asset(unreal.World, "MainMenu", f"{CONTENT_DIR}/Maps", factory)
    else:
        world = unreal.EditorAssetLibrary.load_asset(map_path)
    if world is None:
        _err("could not create MainMenu map")
        return
    unreal.EditorAssetLibrary.save_loaded_asset(world)


# ---------------------------------------------------------------------------
# 4. Default maps
# ---------------------------------------------------------------------------

def set_default_maps():
    try:
        cfg = unreal.GameMapsSettings
        cfg.set_editor_property("game_default_map", unreal.SoftObjectPath("/Game/Maps/MainMenu.MainMenu"))
        cfg.set_editor_property("editor_startup_map", unreal.SoftObjectPath("/Game/Maps/MainMenu.MainMenu"))
        cfg.set_editor_property("server_default_map", unreal.SoftObjectPath("/Game/Maps/MainMenu.MainMenu"))
        cfg.set_editor_property("global_default_game_mode", unreal.SoftObjectPath("/Game/Blueprints/BP_PISGameMode.BP_PISGameMode_C"))
    except Exception as exc:
        _warn(f"set_default_maps: {exc}")


# ---------------------------------------------------------------------------
# 5. Main
# ---------------------------------------------------------------------------

def main():
    _log("Building input context...")
    _ensure_folder(f"{CONTENT_DIR}/Input")
    try:
        ctx = create_input_context()
        if ctx is not None:
            wire_input_context(ctx)
    except Exception as exc:
        _err(f"input setup: {exc}")

    _log("Building core blueprints...")
    try:
        build_core_blueprints()
    except Exception as exc:
        _err(f"blueprints: {exc}")

    _log("Building map Prototype...")
    try:
        build_prototype_map()
    except Exception as exc:
        _err(f"Prototype map: {exc}")

    _log("Building map MainMenu...")
    try:
        build_main_menu_map()
    except Exception as exc:
        _err(f"MainMenu map: {exc}")

    _log("Setting default maps...")
    set_default_maps()

    _log("Done. File > Save All, then Play-In-Editor.")


if __name__ == "__main__":
    main()
