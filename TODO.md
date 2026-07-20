# TODO
## P0 — bloker grywalności
- [ ] **Ręcznie uruchomić `Scripts/ue_build_project.py` w edytorze UE 5.8** (Window > Python). Skrypt tworzy Input Mapping Context, IA assets, GameMode BP, Character BP, NPC BP, Monster BP, mapę `Prototype` z 65 NPC + 6 potworami z JSON. Po zakończeniu: File > Save All, PIE.
- [~] Inventory/quest state/chest + sekwencyjny lockpick: logika C++ gotowa; brakuje UMG widgetów (P1) i BindAction w BP_PISCharacter.
- [~] Combat miecz/łuk/magia, AI, NavMesh: logika C++ gotowa; wymaga importu modeli jako StaticMesh i przypisania do BP_PISCharacter/BP_PISMonster.
- [~] Save/load: `UPISSaveGameSubsystem` z wersjonowanym JSON-em; trzeba dodać UMG Save/Load buttons.
- [ ] Smoke test pionowego wycinka: nowa gra → tutorial → dialog i bezpieczne wyjście → oba łańcuchy kandydackie → nauczyciel → miecz/łuk/czar → zamek → kradzież ze świadkiem → skórowanie → wybór frakcji → epilog → save/load.

## P1 — vertical slice do pełnej gry
- [ ] UMG widgety: HUD (HP/Mana/Stamina + aktywna broń), ekwipunek (kategorie + tooltip), dziennik (active/completed/failed), dialogi (panel + wybory), pauza (zapis/wczytaj/opcje), menu główne. Logika jest w C++ (delegaty `BlueprintAssignable`); WidgetTree'y można zbudować w edytorze lub w `ue_build_project.py`.
- [ ] Opcje: głośność master/music/SFX, rozdzielczość/okno, jakość, czułość myszy, odwrócenie osi opcjonalnie, napisy.
- [ ] Audio: ambienty Rygla, Wolnego Brzegu, Szczeliny; efekty walki, zamka, kroków. Wszystko własne/proceduralne.
- [~] Ikony źródłowe PNG gotowe; zaimportować Texture2D i spiąć z UMG (P1).
- [~] Modele źródłowe OBJ gotowe; zaimportować StaticMesh, UV/lightmap/collision/LOD i rig potworów (P1).
- [ ] Animacje: idle/chód/bieg/obrót/interakcja/rozmowa/sen/dobywanie/ataki/blok/unik/łuk/czar/trafienie/śmierć + 6 stworów.
- [ ] Skórki: proceduralny materiał ciała dla postaci, warianty pancerzy.

## P2
- [ ] Balans, LOD/HLOD, accessibility/options, full smoke regression, packaged Win64 build.
- [ ] Cutscenes/epilog z prostym timeline w UMG.
- [ ] Save/load autosave w bezpiecznych punktach (drzemiący NPC, siedzisko).

## P0 — elementy wykryte w audycie danych
- [x] Zmienić `loot_tables.records[].entries[].item` na `item_id` — zrobione dla wszystkich 8 loot tables; `item` zostawiony jako legacy.
- [x] Rozszerzyć oba walidatory na tablice referencji — `Scripts/validate_json.py` rekurencyjnie sprawdza `item_id`, `npc_id`, `monster_id`, `location_id`, `schedule_id`, `speaker_id`, `quest_id`.
- [x] Dodać podstawowe pola dla broni, pancerzy, roślin, mikstur — zrobione w iteracji 0.4.
- [x] Questy zawierają cele, gałęzie, porażki, nagrody, dziennik — runtime obsługuje Start/Stage/Finish/Progress/Lock/FactionBlock w `UPISQuestComponent`.
- [x] Trzy dialogi JSON — rozszerzono do 68 dialogów (3 ręczne + 65 proceduralnych intro NPC).
- [x] Markery lokacji — dodano 102 markery (8 regionów + 65 bed + 3 work + 3 meal + 3 patrol + spawn/pickup/landmark).
- [x] Loot tables dla 6 stworów — dodano w iteracji 1.
- [x] Statystyki i frakcje potworów — dodane w `monsters.json` (level, hp, damage, faction, pack, aggressive_at_night, trophy_item_id).
