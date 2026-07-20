# Postęp
## Ukończone
- Migracja konfiguracji projektu do **Unreal Engine 5.8** (uproject, Target.cs V7/Unreal5_8, mapy startowe silnika, fix kompilacji pocisku).
- Struktura projektu, dokumentacja, dane kanoniczne JSON i niezależna walidacja.
- 65 nazwanych NPC ze slotami praca/jedzenie/patrol/sen; bazowy zegar C++.
- C++: ID-only inventory, quest state machine, skrzynia z sekwencyjnym zamkiem i pocisk czaru.
- **Iteracja 1 — runtime pionowego wycinka**:
  - `APISCharacter` z Enhanced Input, walką mieczem/łukiem/magią, skórowaniem, interakcją.
  - `APISNPC` + `APISNPCController` — rutyny dzienne ładowane z `npc_schedules.json`, fallback poza widokiem gracza, reakcja na przestępstwa.
  - `APISMonster` + `APISMonsterController` — AI z percepcją, pack behaviour, nocturnal aggression, drop loot ze ściętych tabel.
  - `UPISStatsComponent` (XP, HP, PN, reputacja, factions) z delegatami do UMG.
  - `UPISDialogueComponent` — interpreter JSON z warunkami (flag/quest/item/reputation/skill) i akcjami (set_flag, start_quest, give_item, faction_choice, rep, exit) + `ChooseByIndex`.
  - `UPISCrimeComponent` — świadkowie w promieniu słyszenia/widzenia, eskalacja reakcji.
  - `UPISSaveGameSubsystem` — wersjonowany (schema 1) zapis/odczyt w `Saved/SaveGames`, migracja, lista slotów, usuwanie.
  - `APISGameMode` / `APISGameInstance` z respawn, knockout recovery i wstrzykiwaniem MainMenu na mapie MainMenu.
  - `UPISQuestComponent` z blokadą frakcji (`IsFactionBlocked`).
- **Iteracja 2 — pełne UMG zbudowane w C++** (zero ręcznej pracy w edytorze):
  - `UPISHUDWidget` — HP/Mana bary, level, sól, broń, lokacja, zegar dn./noc, jednorazowa wiadomość.
  - `UPISInventoryWidget` + `UPISInventoryRowWidget` — lista przedmiotów z select/Użyj/Ekwipuj, automatyczne Resolve efektów z JSON (heal/mana).
  - `UPISJournalWidget` — aktywne/ukończone/nieudane, kolorowe sekcje, kroki z JSON `journal`.
  - `UPISDialogueWidget` — pełne UI dialogowe z dynamicznymi przyciskami wyboru, podpięte do `OnNodeShown`.
  - `UPISPauseWidget` — slot zapisu, przyciski Zapisz/Wczytaj, suwaki głośności, opcja odwrócenia osi Y, Resume/Quit.
  - `UPISMainMenuWidget` — Nowa gra / Wczytaj / Opcje / Wyjdź; wstrzykiwany przez `APISGameMode::BeginPlay` na mapie MainMenu.
  - `APISHUD` — most: tworzy widgety, przełącza Inventory/Journal/Pause, obsługuje Esc, podpina Dialogue do Speakera.
  - `APISCharacter::OnInventory/OnJournal/OnPause` z Enhanced Input (klawisze I/J/Esc).
- **`Scripts/ue_build_project.py`**: rozbudowany — tworzy `IA_Inventory`/`IA_Journal`, `BP_PISHUD`, `Maps/MainMenu`, ustawia `MainMenu` jako `GameDefaultMap`. Po uruchomieniu gra startuje od menu.
- 72 własne źródłowe ikony PNG i 5 plansz koncepcyjnych, z pełnym wpisem licencyjnym.
- 60 własnych modeli źródłowych OBJ/MTL dla wyposażenia, stworów i świata; wymagają importu oraz finalnego setupu UE.
- Wymagane minima katalogowe (20 mieczy, 10 łuków, 4 pancerze, 10 roślin, 6 mikstur, 6 potworów, 21 questów, 65 NPC, 68 dialogów, 8 loot tables, 102 markery lokacji).
- **68 dialogów** (3 ręczne + 65 proceduralnych intro NPC) z bezpiecznym wyjściem.
- 6 testów automatycznych: formuły, ekwipunek, flow questa, blokada frakcji, lockpick, leveling.

## Następny krok
P2: import assetów PNG/OBJ do UE, animacje, VFX, audio. Balans.

## W toku / nieuznane za ukończone
- Animacje, VFX, audio — podane jako TODO, assety źródłowe (60 modeli OBJ) są gotowe do importu.
- Import PNG jako Texture2D i OBJ jako StaticMesh (ręczny lub w `ue_build_project.py`).
- Smoke test pionowego wycinka w PIE wymaga działającego edytora UE 5.8.
