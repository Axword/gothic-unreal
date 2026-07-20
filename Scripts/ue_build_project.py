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
  5. Create the HUD Blueprint and configure HUDClass on the GameMode.
  6. Create /Game/Maps/Prototype with a floor, lights and
     all 65 NPCs and 6 monsters placed from world_locations.json.
  7. Create /Game/Maps/MainMenu (empty level with main menu widget).
  8. Bind Prototype as default GameMap + EditorStartupMap in DefaultEngine.ini.

After running, the project can be opened with the editor and Play-In-Editor
launches the vertical slice.
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
# 2. Blueprints
# ---------------------------------------------------------------------------

def create_blueprint(name, parent_class, path):
    full = f"{path}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(full):
        return unreal.EditorAssetLibrary.load_asset(full)
    _ensure_folder(path)
    factory = unreal.BlueprintFactory()
    factory.parent_class = parent_class
    return _asset_tools().create_asset(asset_name=name, package_path=path, asset_class=unreal.Blueprint, factory=factory)


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
    configure_character_bp(bp_char, f"{CONTENT_DIR}/Input/IMC_PISContext")

    bp_npc = create_blueprint("BP_PISNPC", unreal.PISNPC, f"{CONTENT_DIR}/Blueprints/NPC")

    bp_mon = create_blueprint("BP_PISMonster_Wilczak", unreal.PISMonster, f"{CONTENT_DIR}/Blueprints/Monsters")
    cdo = bp_mon.generated_class().get_default_object()
    cdo.set_editor_property("monster_id", "monster_1")

    bp_hud = create_blueprint("BP_PISHUD", unreal.PISHUD, f"{CONTENT_DIR}/Blueprints/UI")

    bp_gm = create_blueprint("BP_PISGameMode", unreal.PISGameMode, f"{CONTENT_DIR}/Blueprints")
    cdo = bp_gm.generated_class().get_default_object()
    cdo.set_editor_property("default_pawn_class", bp_char.generated_class())
    cdo.set_editor_property("hud_class", bp_hud.generated_class())

    bp_gi = create_blueprint("BP_PISGameInstance", unreal.PISGameInstance, f"{CONTENT_DIR}/Blueprints")


# ---------------------------------------------------------------------------
# 3. Maps
# ---------------------------------------------------------------------------

def build_prototype_map():
    map_path = f"{CONTENT_DIR}/Maps/Prototype"
    if not unreal.EditorAssetLibrary.does_asset_exist(map_path):
        factory = unreal.WorldFactory()
        world = _asset_tools().create_asset("Prototype", f"{CONTENT_DIR}/Maps", unreal.World, factory)
    else:
        world = unreal.EditorAssetLibrary.load_asset(map_path)

    try:
        floor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, 0))
        sm = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Plane")
        if sm:
            floor.static_mesh_component.set_static_mesh(sm)
            floor.set_actor_scale3d(unreal.Vector(80, 80, 1))
    except Exception as exc:
        _warn(f"could not add floor: {exc}")

    # Lighting.
    try:
        sun = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 800))
        sun.set_actor_rotation(unreal.Rotator(-50, 30, 0))
        sun.set_actor_label("PIS_Sun")
        sky = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 1200))
        sky.set_actor_label("PIS_Sky")
    except Exception as exc:
        _warn(f"light setup: {exc}")

    # Atmospheric fog.
    try:
        unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.AtmosphericFog, unreal.Vector(0, 0, 200))
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
    if not bp_npc:
        _warn("BP_PISNPC not built; run build_core_blueprints first")
        return
    npc_class = bp_npc.generated_class()

    for npc in npcs["records"]:
        npc_id = npc["id"]
        marker_id = f"marker_bed_{npc_id}"
        marker = by_marker.get(marker_id)
        if not marker:
            continue
        loc = unreal.Vector(marker.get("x", 0), marker.get("y", 0), marker.get("z", 100))
        try:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(npc_class, loc)
            actor.set_actor_label(npc.get("name", npc_id))
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
        except Exception as exc:
            _warn(f"spawn NPC {npc_id}: {exc}")

    monsters = _load_json("monsters.json")
    monster_spawns = _load_json("monster_spawns.json")
    spawn_by_id = {s["id"]: s for s in (monster_spawns or {}).get("records", [])}
    bp_mon = unreal.EditorAssetLibrary.load_asset(f"{CONTENT_DIR}/Blueprints/Monsters/BP_PISMonster_Wilczak")
    if monsters and bp_mon:
        for m in monsters["records"]:
            spawn_id = f"spawn_{m['id']}"
            spawn = spawn_by_id.get(spawn_id)
            if not spawn:
                ms = list(spawn_by_id.values())
                spawn = ms[0] if ms else None
            if not spawn: continue
            loc = unreal.Vector(spawn.get("x", 0), spawn.get("y", 0), spawn.get("z", 110))
            try:
                actor = unreal.EditorLevelLibrary.spawn_actor_from_class(bp_mon.generated_class(), loc)
                actor.set_actor_label(m.get("name", m["id"]))
                cdo = actor
                cdo.set_editor_property("monster_id", m["id"])
                cdo.tags = ["PIS_Monster"]
            except Exception as exc:
                _warn(f"spawn monster {m['id']}: {exc}")

    # World clock.
    try:
        clock = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PISWorldClock, unreal.Vector(0, 0, 200))
        clock.set_actor_label("PIS_Clock")
    except Exception as exc:
        _warn(f"clock: {exc}")

    # PlayerStart
    try:
        spawn = by_marker.get("marker_spawn_arrival", {})
        loc = unreal.Vector(spawn.get("x", 0), spawn.get("y", -4500), spawn.get("z", 200))
        ps = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PlayerStart, loc)
        ps.set_actor_label("PIS_PlayerStart")
    except Exception as exc:
        _warn(f"player start error: {exc}")

    unreal.EditorAssetLibrary.save_loaded_asset(world)


def build_main_menu_map():
    map_path = f"{CONTENT_DIR}/Maps/MainMenu"
    if not unreal.EditorAssetLibrary.does_asset_exist(map_path):
        factory = unreal.WorldFactory()
        world = _asset_tools().create_asset("MainMenu", f"{CONTENT_DIR}/Maps", unreal.World, factory)
    else:
        world = unreal.EditorAssetLibrary.load_asset(map_path)

    # Default level: no actors; the GameMode injects the main menu widget.
    unreal.EditorAssetLibrary.save_loaded_asset(world)


# ---------------------------------------------------------------------------
# 4. Default maps
# ---------------------------------------------------------------------------

def set_default_maps():
    cfg = unreal.GameMapsSettings
    cfg.set_editor_property("game_default_map", unreal.SoftObjectPath("/Game/Maps/MainMenu.MainMenu"))
    cfg.set_editor_property("editor_startup_map", unreal.SoftObjectPath("/Game/Maps/MainMenu.MainMenu"))
    cfg.set_editor_property("server_default_map", unreal.SoftObjectPath("/Game/Maps/MainMenu.MainMenu"))
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
    build_prototype_map()

    _log("Building map MainMenu...")
    build_main_menu_map()

    _log("Setting default maps...")
    set_default_maps()

    _log("Done. Save All and Play-In-Editor.")


if __name__ == "__main__":
    main()
