# TODO
## P0 — bloker grywalności
- [ ] Użyć UE 5.8 Editor do stworzenia `/Game/Maps/Prototype`, GameMode, Enhanced Input i postaci; potem podmienić `GameDefaultMap`/`EditorStartupMap` w `DefaultEngine.ini`.
- [~] Inventory/quest state/chest + sekwencyjny lockpick: logika C++ gotowa; brakuje interakcji, lootu, dialog runtime i UMG.
- [ ] Combat miecz/łuk/magia, AI, NavMesh, 3 NPC routines and 1 monster.
- [ ] Wersjonowany JSON save/load + test round trip.
## P1 — vertical slice do pełnej gry
- [ ] UMG HUD/menu/journal/stats, crime witnesses, trainers, skinning.
- [ ] Wszystkie zadania i dialogi z warunkami/konsekwencjami.
- [~] Ikony źródłowe PNG i koncepcje są gotowe; zaimportować Texture2D i spiąć z UMG.
- [~] Modele źródłowe OBJ istnieją; zaimportować jako StaticMesh, przygotować UV/lightmap/collision/LOD i rig potworów.
- [ ] Dwie osady i pozostałe biomy, authored audio/VFX.
## P2
- [ ] Balans, LOD/HLOD, accessibility/options, full smoke regression and packaged Win64 build.

## P0 — elementy wykryte w audycie danych
- [ ] Zmienić `loot_tables.records[].entries[].item` na `item_id` i rozszerzyć oba walidatory na tablice referencji.
- [~] Dodano podstawowe pola dla broni, pancerzy, roślin i mikstur; uzupełnić odporności/tabele łupów oraz obsługę tych pól runtime.
- [~] Questy zawierają cele, gałęzie, porażki, nagrody i dziennik; wdrożyć ich warunki i konsekwencje w runtime.
- [~] Są trzy dialogi JSON; rozbudować obsadę i zaimplementować interpreter warunków/akcji.
