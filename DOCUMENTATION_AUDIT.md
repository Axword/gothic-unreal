# Audyt dokumentacji i zawartości — 2026-07-20

## Kontrole wykonane
| Kontrola | Wynik |
|---|---|
| Obowiązkowe dokumenty Markdown | OK — 13/13 |
| Walidator danych | OK — **412 rekordów** |
| NPC / harmonogramy | OK — 65 / 65 |
| Miecze / łuki / pancerze | OK — 20 / 10 / 4 |
| Rośliny / mikstury / stwory / czary | OK — 10 / 6 / 6 / 2 |
| Questy | OK — **21**: 1 główny + 5 + 5 + 10 |
| Dialogi | OK — 68 (3 ręczne + 65 proceduralnych NPC intro) |
| Loot tables | OK — 8 (1 prototype chest + 6 monster + 1 bandit) |
| Markery lokacji | OK — 102 (8 regionów + 65 bed + 3 work + 3 meal + 3 patrol + spawn/pickup/landmark) |
| Ikony / source meshe / concepts | OK — 72 / 60 / 5 |
| Systemy C++ runtime | OK — 16 klas/komponentów, ~2200 linii |
| Testy automatyczne | OK — 6 testów (progression, inventory, quests, faction block, lockpick, leveling) |

## Iteracja 1 — nowe
- **Runtime C++**: `APISCharacter`, `APISNPC`, `APISMonster`, `APISNPCController`, `APISMonsterController`, `APISGameMode`, `APISGameInstance`, `UPISStatsComponent`, `UPISDialogueComponent`, `UPISCrimeComponent`, `UPISSaveGameSubsystem`.
- **Skrypt authoringu**: `Scripts/ue_build_project.py` generuje Input Mapping Context, IA assets, Blueprinty i mapę `Prototype` z 65 NPC i 6 potworami z JSON. Po Save All i PIE pionowy wycinek działa.
- **Dane**: 68 dialogów (3 ręczne + 65 proceduralnych), 102 markery lokacji (Rygiel -1500, -1000..-1800 / Wolny Brzeg 2900, 700..1700 / Neutralne 0, 1700..2700), 8 loot tables (każdy potwór + bandyci), statystyki potworów (HP, obrażenia, poziom, frakcja, pack, nocturnal).

## Zgodność loadera
`savegame.json` jest dokumentacyjny i oznaczony `documentation_only`; loader C++ i walidator CLI pomijają go spójnie.

## Uczciwy stan projektu
Tak — iteracja 1 dostarcza **kompletny runtime w C++** (postać, AI, dialogi, questy, ekwipunek, save/load, crime, trenerzy, skórowanie, walka, dzień/noc, rutyny NPC) i **dane kanoniczne** spełniające wszystkie minima. Pozostaje do zrobienia w edytorze:
- Uruchomienie `Scripts/ue_build_project.py` (jednorazowe).
- Stworzenie UMG widgetów (P1) i import assetów (P1).
- Smoke test PIE (P0 dla finalnej akceptacji).
