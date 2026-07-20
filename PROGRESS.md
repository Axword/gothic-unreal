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
  - `UPISDialogueComponent` — interpreter JSON z warunkami (flag/quest/item/reputation/skill) i akcjami (set_flag, start_quest, give_item, faction_choice, rep, exit).
  - `UPISCrimeComponent` — świadkowie w promieniu słyszenia/widzenia, eskalacja reakcji w zależności od reputacji.
  - `UPISSaveGameSubsystem` — wersjonowany (schema 1) zapis/odczyt w `Saved/SaveGames`, migracja, lista slotów, usuwanie.
  - `APISGameMode` / `APISGameInstance` z respawn i knockout recovery.
  - `UPISQuestComponent` z blokadą frakcji (`IsFactionBlocked`).
  - **`Scripts/ue_build_project.py`** — skrypt Python Editor Script, który po uruchomieniu w edytorze UE 5.8 tworzy Input Mapping Context, IA assets, GameMode BP, Character BP, NPC BP, Monster BP, mapę `Prototype` z 65 NPC + 6 potworami rozmieszczonymi z JSON i ustawia domyślną mapę.
- 72 własne źródłowe ikony PNG i 5 plansz koncepcyjnych, z pełnym wpisem licencyjnym.
- 60 własnych modeli źródłowych OBJ/MTL dla wyposażenia, stworów i świata; wymagają importu oraz finalnego setupu UE.
- Wymagane minima katalogowe (20 mieczy, 10 łuków, 4 pancerze, 10 roślin, 6 mikstur, 6 potworów, 21 questów, 65 NPC, 68 dialogów, 8 loot tables, 102 markery lokacji).
- **68 dialogów** (3 ręczne + 65 proceduralnych intro NPC) z bezpiecznym wyjściem.
- 5 nowych testów automatycznych: formuły, ekwipunek, flow questa, blokada frakcji, lockpick, leveling.

## Następny krok
P1: ręcznie uruchomić `Scripts/ue_build_project.py` w edytorze UE 5.8 (Window > Python), co wygeneruje mapę `Prototype` i wszystkie Blueprinty. Potem: PIE i smoke test pionowego wycinka.

## W toku / nieuznane za ukończone
- UMG widgety (HUD, ekwipunek, dziennik, dialogi, menu pauzy) — logika w C++ jest gotowa (komponenty z delegatami `BlueprintAssignable`), ale same WidgetTree'y trzeba utworzyć w edytorze lub rozbudować `ue_build_project.py`.
- Animacje, VFX, audio — podane jako TODO, assety źródłowe (60 modeli OBJ) są gotowe do importu.
- Import PNG jako Texture2D i OBJ jako StaticMesh (ręczny lub w `ue_build_project.py`).
- Smoke test pionowego wycinka w PIE wymaga działającego edytora UE 5.8.
