# TODO
## P0 — bloker grywalności
- [ ] Użyć UE Editor do stworzenia `/Game/Maps/Prototype`, GameMode, Enhanced Input i postaci.
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
- [ ] Rozbudować wszystkie rekordy przedmiotów o gameplayowe pola wymagane w specyfikacji (obrażenia, wymagania, masa, efekty); obecny katalog jest głównie indeksem zawartości.
- [ ] Nadać 21 questom właściwe cele, warunki, alternatywne rozwiązania, porażki, nagrody i wpisy dziennika; obecne rekordy są szkieletami.
- [ ] Rozbudować dialogi poza jednym prototypem oraz implementować interpreter warunków i akcji.
