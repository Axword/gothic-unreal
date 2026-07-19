# TODO
## P0 — bloker grywalności w edytorze UE
- [ ] Użyć UE Editor do stworzenia `/Game/Maps/Prototype`, GameMode, Enhanced Input i postaci.
- [~] Inventory/quest state/chest + sekwencyjny lockpick: logika C++ i dane gotowe (przetestowane w harnessie i `test_suite.py`); brakuje widżetów UMG i Blueprintów interakcji w edytorze.
- [ ] Osadzenie combat miecz/łuk/magia, AI, NavMesh i animacji na postaci w edytorze.
- [x] Wersjonowany JSON save/load + automatyczny test round trip (`Scripts/smoke_test.py` i `test_suite.py`).

## P1 — vertical slice do pełnej gry
- [ ] UMG HUD/menu/journal/stats i wizualizacja w świecie.
- [x] Pełna sieć dialogów dla 65 NPC, nauczycieli, handlarzy i kradzieży w JSON (`dialogues_prototype.json`).
- [x] Wszystkie 22 zadania i dialogi z warunkami, nagrodami, wykluczającym wyborem frakcji (`stary_porzadek` vs `nowy_porzadek`) i epilogiem.
- [x] Dwie osady, lokacje, 6 potworów o zróżnicowanych statystykach, tabele lootu i skórowanie.
- [ ] Autorskie assety wizualne, audio i VFX w edytorze UE.

## P2
- [x] Balans formuł rozwoju, broni, zbroi, czarów i potworów w danych JSON.
- [x] Kompletny zestaw testów automatycznych Python/C++ i regresyjny smoke test 18 kroków.
- [ ] LOD/HLOD, accessibility/options i spakowany build Win64 (wymaga edytora).
