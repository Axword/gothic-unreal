# Changelog
## 2026-07-19 — Iteration 0.2 (Agent nr 2 - Audyt techniczny, weryfikacja i naprawa)
- **Walidator i silnik danych**: Rozbudowano `Scripts/validate_json.py` oraz `UPISJsonDataSubsystem.cpp` o pełne sprawdzanie zagnieżdżonych referencji w listach/obiektach i weryfikację pól wymaganych dla każdej domeny.
- **Balans i statystyki**: Uzupełniono 20 mieczy (`damage`, `required_strength`), 10 łuków (`damage`, `required_dexterity`), 4 zbroje (`defense`, `required_strength`), 6 potworów (`hp`, `damage`, `xp`, `trophy_id`, `loot_table_id`) i 2 czary (`damage`, `mana`, `cast_time`). Stworzono tabele lootu w `loot_tables.json`.
- **Questy i Dialogi**: Zastąpiono atrapy tekstowe w zadaniach kandydackich (`old_trial_1..5`, `new_trial_1..5`) i pobocznych (`side_1..10`) autentycznymi celami (`objectives`) i nagrodami. Utworzono 8 pełnych drzew dialogowych dla 65 NPC w `dialogues_prototype.json`, obsługujących wykluczający wybór frakcji i epilog.
- **Lokacje i harmonogramy**: Zmapowano `location_id` na wszystkich slotach w `npc_schedules.json` oraz dodano lokację fallback `teleport_out_of_sight` do `world_locations.json`.
- **Logika C++**:
  - `PISQuestComponent`: Dodano pełną idempotencję nagród i metodę `ChooseFaction` blokującą zadania przeciwnej frakcji (`EPISQuestStatus::Locked`).
  - `PISInventoryComponent`: Dodano weryfikację wymagań siły/zręczności (`EquipWeaponWithStats`).
  - `PISLockedChest`: Dodano sprawdzanie posiadania wytrychów przed otwieraniem oraz przerywanie przy zerowym stanie.
  - `PISWorldClock`: Naprawiono zwiększanie dnia (`Day`) podczas snu przez północ.
- **Testy automatyczne**: Stworzono test runner w Pythonie `Scripts/test_suite.py` (19 kategorii, 100% PASS), harness C++ `Scripts/run_cpp_harness.cpp` (30 testów, 100% PASS) oraz kompletny test integracyjny pętli gry `Scripts/smoke_test.py` weryfikujący 18 kroków vertical slice'a.

## 2026-07-19 — Iteration 0.1
- Dodano komponenty ekwipunku i przejść questów, z eventem dla UMG/Blueprintów.
- Dodano aktora skrzyni z trzyruchowym zamkiem L/P i kosztem wytrycha oraz pocisk czaru z obrażeniami.
- Dodano testy automatyczne inventory i przepływu questa.

## 2026-07-19 — Iteration 0
- Utworzono projekt UE 5.4 C++ i fundament JSON.
- Dodano 243 rekordy: 65 nazwanych NPC i harmonogramy, katalog przedmiotów, 6 stworów, questy, lokacje, trenerów i czary.
- Dodano loader runtime, walidator, zegar świata i test formuł.
- Zdefiniowano oryginalny świat Popiołu i Soli.
