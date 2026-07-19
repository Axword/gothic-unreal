# Dane i ID

## Kontrakt wspólny
Każdy katalog danych ma root `{ "schema_version": 1, "records": [] }`; rekord ma globalnie unikalne ID `snake_case`. `Content/Data/Schemas/record_file.schema.json` opisuje wspólny root, a `Scripts/validate_json.py` sprawdza składnię, duplikaty i referencje pojedynczych pól kończących się `_id`. `UPISJsonDataSubsystem` robi równoważną kontrolę przy starcie UE i zapisuje błędy do logu.

`savegame.json` ma `documentation_only: true`: jest przykładem kontraktu zapisu i jest pomijany przez oba loadery. Runtime state nie jest danymi kanonicznymi.

## Katalogi
| Plik | Rekordy / przeznaczenie |
|---|---|
| `items_weapons_swords`, `items_weapons_bows`, `items_armors` | wyposażenie; `asset_path` oznacza docelowy Texture2D ikony, `source_mesh_path` — importowalny OBJ |
| `items_plants`, `items_potions`, `items_trophies`, `items_misc` | consumables, materiały, waluta i klucze |
| `npcs`, `npc_schedules` | 65 osób i sloty pracy/jedzenia/patrolu/snu |
| `monsters`, `monster_spawns` | archetypy potworów i ich regiony |
| `quests_*`, `dialogues_*` | definicje questów, etapów i węzłów rozmów |
| `world_locations`, `loot_tables`, `trainers`, `spells`, `balance` | świat, łup, trening, magia i formuły |

## Konwencje referencji
`npc_id`, `monster_id`, `location_id`, `schedule_id` i `speaker_id` są obowiązkowo resolvowalne do rekordu. `item` w tabeli lootu jest obecnie nazwą pola legacy i nie jest jeszcze objęty automatyczną walidacją — należy go zmienić na `item_id` przy implementacji lootu. To jest znane P0 jakości danych.

## Kontrakt zapisu docelowego
`schema_version`, pozycja/czas gracza, statystyki, inventory/equipment ID, quest states, flagi, reputacje, NPC state, pokonani wrogowie, chest state, zabrany loot, rutyny i wybór frakcji. Brak wskaźników UObject. Migracja: nieznane pola ignoruj, brakujące wypełniaj wartością domyślną, nowszą wersję odrzuć z czytelnym błędem.
