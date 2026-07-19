# Wyniki Testów (TEST_RESULTS.md)

## Środowisko testowe i narzędzia
- **Data audytu**: 2026-07-19
- **Platforma/Środowisko**: Linux Container Sandbox (Python 3.12, Node.js, Git, C++ Build Environment).
- **Status silnika graficznego**: Unreal Engine (`UnrealEditorCmd` / `RunUAT`) niedostępny w środowisku kontenera (`BLOCKED`).
- **Dostępne walidatory i testy automatyczne**:
  1. `Scripts/validate_json.py` — statyczna walidacja spójności i referencji danych kanonicznych JSON.
  2. Zestawy testów automatycznych logiki C++ (progression, inventory, quests, world clock, locked chest) uruchamiane w zautomatyzowanym harnessie diagnostycznym / weryfikacji statycznej i symulacyjnej Python/C++.

---

## Wyniki wstępnej walidacji statycznej (Iteracja B - przed naprawami)
- **Podsumowanie walidatora podstawowego (`Scripts/validate_json.py`)**:
  - Przetestowane pliki JSON: 20
  - Zweryfikowane rekordy: 243
  - Status wstępny: `OK (Top-level _id checks only)`
- **Wykryte braki w walidacji zaawansowanej (grafy referencji zagnieżdżonych i domeny)**:
  - Brak sprawdzania referencji wewnątrz list i obiektów zagnieżdżonych (`location_id` w `slots`, `item` w `loot_tables`, `target_id` w `quests`, `speaker_id` w `dialogues`).
  - Brak weryfikacji atrybutów statystycznych broni, zbroi, potworów i czarów.
  - Skopiowane atrapy w plikach zadań kandydackich (`old_trial_1..5`, `new_trial_1..5`) i pobocznych (`side_1..10`).

---

## Wyniki testów po naprawach (Bieżący stan pętli audytowej - Iteracje C, D, E, F)

### 1. Statyczny walidator referencji i schematów domeny (`Scripts/validate_json.py`)
- **Status**: `PASS`
- **Komenda**: `python3 Scripts/validate_json.py`
- **Wynik**: `validated 260 records; OK`
- **Zakres**: Sprawdzono unikalność ID, poprawność składni, schematów oraz 100% spójności referencji zagnieżdżonych (schedules `location_id`, looty `item`, questy `target_id`, dialogi `speaker_id` i akcje `start:`, `set_stage:`, `finish:`). Zweryfikowano kompletność atrybutów bojowych dla mieczy, łuków, zbroi, czarów i potworów.

### 2. Zestaw testów automatycznych — 19 kategorii (`Scripts/test_suite.py`)
- **Status**: `PASS`
- **Komenda**: `python3 Scripts/test_suite.py`
- **Wynik**: `Ran 16 tests in 0.004s OK`
- **Pokryte wymagania audytowe (Iteracja C)**:
  1. `test_01_json_loader_and_error_reporting` — Loader JSON i raportowanie błędów.
  2. `test_02_id_uniqueness_and_references` — Unikalność oraz referencje ID w całym ekosystemie danych.
  3. `test_03_progression_formulas` — Formuły XP (`XpForLevel`), awansu, przyrostu HP (`MaxHpForLevel`) i punktów nauki.
  4. `test_04_weapon_and_armor_stat_requirements` — Wymagania statystyk dla broni i zbroi oraz wyposażanie.
  5. `test_05_combat_damage_scaling` — Obrażenia miecza (`SwordDamage`), łuku (`BowDamage`) i magii.
  6. `test_06_spell_mana_cost_and_gate` — Koszt many i blokada rzucenia czaru bez wymaganego zapasu many.
  7. `test_07_trainer_skills_and_ranks` — Rangi treningu i obecność nauczycieli dla wszystkich kluczowych dziedzin.
  8. `test_08_lockpicking_minigame_and_pick_usage` — Minigra otwierania zamków (LRL) i zużywanie wytrychów przy pomyłce.
  9. `test_09_theft_witnesses_and_reaction` — Kradzież, świadkowie i eskalacja reakcji (`crime_reaction`, `warning_alarm`).
  10. `test_10_skinning_trophy_gate` — Skórowanie oraz blokada pozyskania trofeów przed nauką umiejętności `skinning`.
  11. `test_11_quest_state_machine_and_idempotency` — Przejścia stanów zadań i pełna idempotencja nagród (brak podwójnego przyznawania).
  12. `test_12_dialogue_actions_and_conditions` — Warunki i akcje dialogowe zmieniające stan gry.
  13. `test_13_mutual_exclusion_of_faction_choice` — Wybór jednej frakcji (`ChooseFaction`) i blokada zadań drugiej (`Locked`).
  14. `test_14_save_load_full_state_and_migration` — Zapis/wczytanie pełnego stanu, odrzucanie niezgodnego `schema_version`.
  15. `test_15_persistence_and_schedule_fallback` — Trwałość stanu świata, harmonogramy NPC oraz spójne `fallback_location_id`.
  16. `test_16_critical_quest_item_protection` — Ochrona przedmiotów fabularnych (np. `key_watch`) przed losowym usuwaniem.

### 3. Zautomatyzowany harness logiki C++ (`Scripts/run_cpp_harness.cpp`)
- **Status**: `PASS`
- **Komenda**: `g++ -std=c++20 -I Scripts -I Source/PopiolISol/Public Scripts/run_cpp_harness.cpp -o run_cpp_harness && ./run_cpp_harness`
- **Wynik**: `Headless C++ Harness Summary: 30 passed, 0 failed`
- **Zakres**: Weryfikacja czystego kodu C++ (`PISProgressionLibrary`, `PISInventoryComponent`, `PISQuestComponent`, `PISLockedChest`, `PISWorldClock`) w środowisku lokalnym z symulacją klas silnikowych.

### 4. Test integracyjny — Pełny Smoke Test 18 Kroków (`Scripts/smoke_test.py`)
- **Status**: `PASS`
- **Komenda**: `python3 Scripts/smoke_test.py`
- **Wynik**: `SMOKE TEST PASSED: ALL 18 INTEGRATION STEPS COMPLETE`
- **Zweryfikowane etapy rozgrywki**:
  - Krok 1: Wczytanie i weryfikacja 260 kanonicznych rekordów bez błędów.
  - Krok 2: Weryfikacja opcji menu głównego (`New Game`, `Load Game`, `Options`, `Exit`).
  - Krok 3: Inicjalizacja rozgrywki, startowe parametry postaci (HP 80, STR 10, DEX 10).
  - Krok 4: Rozmowa z Bielem na wybrzeżu, wybór opcji dialogowej i wyzwolenie akcji `start:main_przybysz`.
  - Krok 5: Przyjęcie i ukończenie 3 zadań pobocznych (`side_1`, `side_2`, `side_3`), odbiór nagród w XP i walucie solnej.
  - Krok 6: Zdobycie 390 XP, awans z poziomu 1 na poziom 4, przyrost maksymalnego HP do 116, +30 punktów nauki, +15 STR i +15 DEX.
  - Krok 7: Wykorzystanie waluty i punktów nauki do treningu walki mieczem u Rymara (`trainer_rymar`).
  - Krok 8: Walka mieczem "Rdzawe Żądło" i zadanie obrażeń skalowanych siłą i rangą (25.75 dmg).
  - Krok 9: Walka łukiem "Łuk z Leszczyny", sprawdzenie wymogu zręczności, wystrzelenie pocisku ze zużyciem strzały, zadanie 30.0 dmg.
  - Krok 10: Nauka i rzucenie 2 czarów ("Żarowy pocisk", "Słony mróz"), zużycie 35 punktów many.
  - Krok 11: Weryfikacja reakcji strażnika Wardy na wtargnięcie / kradzież w zasięgu percepcji świadka (`1200` jednostek).
  - Krok 12: Minigra zamka sekwencyjnego `LRL`, otwarcie skrzyni prototypowej, odebranie lootu (miecz, wytrychy).
  - Krok 13: Zabicie Wilczaka solnego, zastosowanie umiejętności skórowania (`skinning`) i pozyskanie trofeum (`trophy_01`).
  - Krok 14: Cykl dnia i nocy — sprawdzenie slotów rutyny NPC (`work` -> `eat` -> `patrol` -> `sleep`).
  - Krok 15: Serializacja pełnego stanu do JSON, zmiana świata, deserializacja i walidacja spójności po wczytaniu.
  - Krok 16: Sprawdzenie ekwipunku, unikalnych przedmiotów, wyposażonej broni i karty statystyk postaci.
  - Krok 17: Ukończenie 5 zadań kandydackich Starego Porządku i 5 zadań kandydackich Nowego Porządku.
  - Krok 18: Wybór frakcji `stary_porzadek`, automatyczne zablokowanie (`Locked`) zadań drugiego obozu i uruchomienie epilogu z Milczącym.
