# Popiół i Sól

Oryginalny, polskojęzyczny action RPG 3D dla **Unreal Engine 5.8**. Iteracja 1 dostarcza kompletny runtime w C++ (postać, walka, dialogi, AI, crime, save/load, trenerzy, skórowanie, rutyny NPC) oraz dane kanoniczne (65 NPC, 68 dialogów, 21 questów, 20 mieczy, 10 łuków, 4 pancerze, 6 stworów, 2 czary, 8 loot tables, 102 markery lokacji). Skrypt `Scripts/ue_build_project.py` po uruchomieniu w edytorze UE 5.8 automatycznie generuje Input Mapping Context, Blueprinty (Character, NPC, Monster, GameMode, GameInstance) oraz mapę `Prototype` z 65 NPC i 6 potworami rozmieszczonymi z JSON. Po Save All można uruchomić PIE.

## Co jest w repozytorium
- **C++ kompletny runtime** (`Source/PopiolISol/`): `APISCharacter`, `APISNPC`, `APISMonster`, `UPISStatsComponent`, `UPISDialogueComponent`, `UPISCrimeComponent`, `UPISQuestComponent`, `UPISInventoryComponent`, `UPISSaveGameSubsystem`, `APISGameMode`, `APISGameInstance`, `APISNPCController`, `APISMonsterController`, `APISWorldClock`, `UPISJsonDataSubsystem`, `APISSpellProjectile`, `APISLockedChest`, `UPISProgressionLibrary` + 5 testów automatycznych.
- **JSON jako kanoniczne dane świata** (`Content/Data/Json/`): 412 rekordów, walidowane przez `Scripts/validate_json.py`.
- **Skrypt authoringu** (`Scripts/ue_build_project.py`): generuje Input, Blueprinty i mapę w edytorze.
- **72 własne ikony PNG**, **60 source mesh OBJ/MTL**, **5 plansz koncepcyjnych** (wszystkie własne, procedurally generated).

## Wymagania (UE 5.8)
- **Unreal Engine 5.8** (Epic Launcher lub build ze źródeł).
- **Visual Studio 2022 lub 2026** z workloadem *Desktop development with C++* / *Game development with C++*.
- Preferowany toolset MSVC wskazany przez UE 5.8.
- **Python Editor Script Plugin** (wbudowany w UE 5.8; włącz: Edit > Plugins > Scripting > Python Editor Script Plugin).

## Uruchomienie i weryfikacja
1. **Walidacja danych** (terminal):
   ```
   python3 Scripts/validate_json.py
   ```
   Oczekiwany wynik: `validated 412 records; OK`.

2. **Kompilacja C++**:
   - PPM na `PopiolISol.uproject` → *Generate Visual Studio project files*.
   - Otwórz `PopiolISol.sln` w Visual Studio, zbuduj `PopiolISolEditor` (Development Editor, Win64).
   - Uruchom `PopiolISolEditor.exe` (lub kliknij dwukrotnie `.uproject`).

3. **Generowanie mapy i blueprintów** (w edytorze):
   - Otwórz projekt w edytorze UE 5.8.
   - `Window > Python > py Scripts/ue_build_project.py`.
   - Skrypt tworzy Input Mapping Context + IA assets, Blueprinty postaci/NPC/potwora/GameMode/GameInstance, mapę `Prototype` z 65 NPC + 6 potworami rozmieszczonymi z JSON i ustawia `GameDefaultMap`/`EditorStartupMap`.

4. **Smoke test pionowego wycinka**:
   - `File > Save All`.
   - `Play > Play In Editor` (lub Alt+P).
   - W grze: WASD ruch, mysz kamera, E interakcja, LPM atak (miecz/łuk/czar w zależności od trybu), Spacja unik, Q czar, Esc pauza.
   - Pierwszy NPC: `Biel` (`npc_neu_01`) — pomaga przeczytać list i uruchamia `main_przybysz`.
   - `Rymar` w Ryglu (`npc_sta_02`) uczy miecza, `Nita` w Wolnym Brzegu (`npc_now_01`) uczy kradzieży.

5. **Testy automatyczne** (Session Frontend > Automation):
   - `PopiolISol.Progression.Formulas` — XP/HP/obrażenia.
   - `PopiolISol.Quests.Transitions` — start/etap/finał.
   - `PopiolISol.Inventory.Stacks` — stack/remove.
   - `PopiolISol.Quests.FactionBlock` — blokada przeciwnej frakcji.
   - `PopiolISol.Chest.Lockpick` — sukces, błąd, utrata wytrycha.
   - `PopiolISol.Stats.Leveling` — XP do poziomu, LP, MaxHP.

## Docelowe sterowanie
WASD — ruch; mysz — kamera; E — interakcja (NPC/chest/monster); LPM — atak (miecz: sweep; łuk: przytrzymaj i puść; magia: pocisk); PPM — blok/celowanie; Spacja — unik; Q — czar; Esc — pauza; I — ekwipunek (TODO UMG P1); J — dziennik (TODO UMG P1).

## Status i licencje
Stan realizacji jest w `PROGRESS.md`; bezwzględne braki w `TODO.md`. `THIRD_PARTY_ASSETS.md` rejestruje pochodzenie assetów. Dane kanoniczne są w `Content/Data/Json`; zapis runtime ma trafić do `Saved/SaveGames`, nigdy do `Content`.
