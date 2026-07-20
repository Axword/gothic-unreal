# Architektura
`UPISJsonDataSubsystem` ładuje `Content/Data/Json` przy starcie, wykrywa duplikaty oraz referencje `*_id`, logując plik/ID. Pythonowy walidator jest szybkim testem CI. JSON jest źródłem prawdy; Blueprinty są tylko prezentacją i kompozycją. `APISWorldClock` dostarcza czas/dzień/noc. `UPISProgressionLibrary` zawiera czyste, testowalne formuły.

## Runtime (iteracja 1)
**GameInstance** (`APISGameInstance`): utrzymuje `UPISJsonDataSubsystem` (loader danych kanonicznych) i `UPISSaveGameSubsystem` (zapis/odczyt wersjonowanego JSON w `Saved/SaveGames`).

**GameMode** (`APISGameMode`): `APISCharacter` jako `DefaultPawnClass`. Respawn gracza na spawn marker po śmierci.

**Postać gracza** (`APISCharacter`):
- `UPISStatsComponent` — XP/HP/Mana/LearningPoints, `FPISPrimaryStats` (Siła/Zręczność/Witalność/ManaStat), `FPISSkillRanks` (7 umiejętności), `Reputation` (mapa faction→int), `FactionChoice`.
- `UPISInventoryComponent` — `Items: TArray<FPISInventoryStack>`, `EquippedWeaponId`, Add/Remove/Count/Equip.
- `UPISQuestComponent` — `TArray<FPISQuestState>` ze statusem/stage/progress; StartQuest/SetStage/FinishQuest/SetProgress/AddProgress/LockQuest/IsFactionBlocked. Faction blocking: heurystyczny, sprawdza ukończenie całego łańcucha.
- `UPISDialogueComponent` — interpreter JSON: warunki (flag/quest/item/reputation/skill) i akcje (set_flag/clear_flag/start_quest/set_stage/finish_quest/give_item/take_item/faction_choice/rep/exit).
- `UPISCrimeComponent` — świadkowie w promieniu, eskalacja reakcji.
- Enhanced Input — Move/Look/Interact/Attack/Block/Dodge/Cast/Pause, podpięte w `SetupPlayerInputComponent`.
- Combat: `PerformSwordAttack` (sweep), `BeginBowDraw/ReleaseBowShot` (line trace), `CastActiveSpell` (spawn `APISSpellProjectile`). Skórowanie: `SkinMonster` po knockdownie.

**NPC** (`APISNPC` + `APISNPCController`):
- `APISNPC::ApplyFromJson` w `BeginPlay` ustawia Faction/Role/Schedule/Attitude/CrimeReaction.
- `APISNPC::MoveToMarker` — szuka `marker_<name>` w `GetAllRecords()`, jeśli NavMesh dostępny `SimpleMoveToLocation`, wpp. teleportuje poza widok gracza.
- `APISNPCController::Tick` co klatkę sprawdza `APISWorldClock` i w razie zmiany godziny/dnia odświeża slot aktywności (praca/jedzenie/patrol/sen).

**Potwory** (`APISMonster` + `APISMonsterController`):
- `APISMonster::BeginPlay` wczytuje HP/damage/level/faction/pack/nocturnal z JSON, ustawia `PatrolOrigin`.
- `APISMonster::ReceiveDamage` z pancerzem; pierwsza śmiertelna dawka → `Downed` (nie Dead), druga → `Dead` + `DropLoot` (z `loot_<monster_id>`).
- `APISMonsterController::EvaluateSenses` — skanuje `APISCharacter` w zasięgu wzroku/słuchu, night-only aggression dla `monster_5`, pack behavior dla `monster_1` (taguje cel `PIS_Hostile`).
- `GetTeamAttitudeTowards` — sojusz vs frakcja, wrogość wobec gracza.

**Zapis/odczyt** (`UPISSaveGameSubsystem`):
- `schema_version=1`, JSON w `Saved/SaveGames/pis_save_<slot>.json`.
- Round-trip: pozycja, rotacja, level/xp/lp/hp/mana, primary/skills/reputation/faction_choice, inventory (items + equipped), quests (id/stage/status/progress).
- Migracja: nieznane pola ignorowane, nierozpoznana wersja odmówiona z komunikatem.
- `ListSlots`, `DeleteSlot`, `GetLastError`.

**Plan authoringu w edytorze**:
1. Uruchom `Scripts/ue_build_project.py` (Window > Python w edytorze).
2. Skrypt tworzy `Input/IMC_PISContext`, `IA_*`, `Blueprints/Player/BP_PISCharacter`, `Blueprints/NPC/BP_PISNPC`, `Blueprints/Monsters/BP_PISMonster_Wilczak`, `BP_PISGameMode`, `BP_PISGameInstance`, oraz `Maps/Prototype` z 65 NPC + 6 potworami rozmieszczonymi z JSON.
3. Ustawia `GameDefaultMap`/`EditorStartupMap` na `/Game/Maps/Prototype`.
4. Zapisz wszystko (File > Save All). PIE uruchamia pionowy wycinek.

## Plan runtime danych
GameInstance (dane/save) → GameMode (spawn) → Character components (inventory, combat, quests, dialogue, crime) → AI controller/schedule service (NPC + monster) → UMG (HUD/inventory/journal/dialogue/pause). Save JSON posiada wersję, stabilne ID i wartości domyślne; nigdy wskaźniki UE.
