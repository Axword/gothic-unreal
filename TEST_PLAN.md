# Test plan

## Aktualny stan testów
| Test | Status |
|---|---|
| `python3 Scripts/validate_json.py` — JSON, duplikaty, pojedyncze `*_id` | wykonany: OK, 243 rekordy |
| `PopiolISol.Progression.Formulas` — XP/HP/obrażenia | napisany, niewykonany (brak UE Editor) |
| `PopiolISol.Quests.Transitions` — start/etap/finał | napisany, niewykonany |
| `PopiolISol.Inventory.Stacks` — stack/remove | napisany, niewykonany |

## Brakujące automatyzacje P0
- loader: nieprawidłowy JSON, brak `records`, duplikat i brak referencji;
- loot table `item_id` po migracji kontraktu;
- lockpick: sukces, błąd, utrata wytrycha i gate rangi;
- save/load: round-trip, wersja nieobsługiwana i wartości domyślne;
- quest: blokada przeciwnej frakcji i brak soft-locka.

## Smoke po implementacji UE
Nowa gra → tutorial → dialog i bezpieczne wyjście → oba łańcuchy kandydackie → nauczyciel → miecz/łuk/czar → zamek → kradzież ze świadkiem → skórowanie → wybór frakcji → epilog → save/load. Regresja: ważny NPC/quest item nie znika przez walkę/rutynę, zablokowany NavMesh używa fallbacku poza widokiem, a nieistniejący ID daje log z plikiem i rekordem.
