# Changelog
## 2026-07-20 — Iteracja 2: pełne UMG zbudowane w C++
- **8 nowych klas UMG** (`Source/PopiolISol/Public/`, `Private/`):
  - `UPISHUDWidget` — HP/Mana bary, level, sól, broń, lokacja, zegar dn./noc, jednorazowa wiadomość.
  - `UPISInventoryWidget` + `UPISInventoryRowWidget` — ekwipunek z select/Użyj/Ekwipuj, Resolve efektów z JSON.
  - `UPISJournalWidget` — aktywne/ukończone/nieudane questy z tekstami etapów z JSON.
  - `UPISDialogueWidget` — pełne UI dialogowe z dynamicznymi wyborami.
  - `UPISPauseWidget` — zapis/wczytanie + suwaki głośności + opcje.
  - `UPISMainMenuWidget` — menu główne.
  - `APISHUD` — most: tworzy widgety, podpina Dialogue, przełącza Inventory/Journal/Pause.
- **`APISCharacter`**: nowe wejścia I (Inventory) i J (Journal) przez Enhanced Input, handler `OnInventory`/`OnJournal`. Po interakcji z NPC auto-otwiera `UPISDialogueWidget` przez `APISHUD::ShowDialogueFor`.
- **`APISGameMode`**: HUDClass ustawiony na `APISHUD`; `BeginPlay` wstrzykuje `UPISMainMenuWidget` na mapie `MainMenu`.
- **`UPISDialogueComponent`**: nowa metoda `ChooseByIndex(int32)` — wygodne wywołanie z UI po indeksie widocznego wyboru.
- **`Scripts/ue_build_project.py`**: dodano `IA_Inventory` (I), `IA_Journal` (J), tworzenie `BP_PISHUD`, mapy `Maps/MainMenu`, ustawienie `MainMenu` jako `GameDefaultMap`/`EditorStartupMap`.
- **`Source/PopiolISol/PopiolISol.Build.cs`**: dodano `ApplicationCore` dla bezpieczeństwa linkowania UI.

## 2026-07-20 — Iteracja 1: runtime pionowego wycinka
- **Nowe systemy C++ (kompletny gameplay)**: `APISCharacter` (Enhanced Input, miecz/łuk/magia, skórowanie, interakcja), `APISNPC` + `APISNPCController` (rutyny dzienne z JSON, reakcja na przestępstwa), `APISMonster` + `APISMonsterController` (AI z percepcją, pack, nocturnal, drop loot), `UPISStatsComponent` (XP/HP/PN, reputacja, factions), `UPISDialogueComponent` (interpreter JSON z warunkami i akcjami: flag, quest, item, reputation, skill), `UPISCrimeComponent` (świadkowie, eskalacja), `UPISSaveGameSubsystem` (wersjonowany JSON save/load + migracja), `APISGameMode`/`APISGameInstance`.
- **Rozbudowa danych**: dialogi dla wszystkich 65 NPC, markery lokacji rozmieszczone w Ryglu i Wolnym Brzegu, loot tables dla 6 stworów + bandytów, statystyki i frakcje potworów, brakujący `food_bread` w `items_misc.json`.
- **`Scripts/ue_build_project.py`**: skrypt Python Editor Script, który po uruchomieniu w edytorze UE 5.8 tworzy Input Mapping Context, GameMode BP, Character BP, NPC BP, Monster BP, mapę `Prototype` z 65 NPC i 6 potworami rozmieszczonymi z JSON oraz ustawia domyślną mapę.
- **Nowe testy automatyczne**: `PopiolISol.Quests.FactionBlock`, `PopiolISol.Chest.Lockpick`, `PopiolISol.Stats.Leveling` (`Source/PopiolISol/Private/Tests/PISSaveLoadTests.cpp`).
- **Build dependencies**: dodano `GameplayTasks` w `PopiolISol.Build.cs` dla `UAISenseConfig_*` i `UAIPerceptionComponent`.
- **Loader ulepszony**: `UPISJsonDataSubsystem` ma teraz publiczne `GetAllRecords()` potrzebne do iteracji markerów NPC; walidator CLI zaakceptował 412 rekordów.

## 2026-07-20 — Migracja UE 5.8
- `EngineAssociation` przestawione z `5.4` na `5.8`.
- `*.Target.cs`: `BuildSettingsVersion.V7` oraz `EngineIncludeOrderVersion.Unreal5_8`.
- `DefaultEngine.ini` nie wskazuje już nieistniejącej `/Game/Maps/Prototype` (używa map silnika Entry/Template, żeby edytor otwierał się bez błędu brakującej mapy). Po uruchomieniu `Scripts/ue_build_project.py` wskazuje na `/Game/Maps/MainMenu`.
- Naprawiono sygnaturę `APISSpellProjectile::OnImpact` pod `FComponentHitSignature`.
- Włączono plugin Enhanced Input w `.uproject`; dodano `.gitignore` i `DefaultGame.ini`.

## 2026-07-19 — Iteration 0
- Utworzono projekt UE 5.4 C++ i fundament JSON.
- Dodano 243 rekordy: 65 nazwanych NPC i harmonogramy, katalog przedmiotów, 6 stworów, questy, lokacje, trenerów i czary.
- Dodano loader runtime, walidator, zegar świata i test formuł.
- Zdefiniowano oryginalny świat Popiołu i Soli.

## 2026-07-19 — Iteration 0.1
- Dodano komponenty ekwipunku i przejść questów, z eventem dla UMG/Blueprintów.
- Dodano aktora skrzyni z trzyruchowym zamkiem L/P i kosztem wytrycha oraz pocisk czaru z obrażeniami.
- Dodano testy automatyczne inventory i przepływu questa.

## 2026-07-19 — Iteration 0.2 / 0.3 / 0.4
- Źródła wizualne (72 PNG, 60 OBJ/MTL), audyt dokumentacji, dane grywalnego przepływu.
