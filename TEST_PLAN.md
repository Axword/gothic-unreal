# Test plan

## Aktualny stan testów
| Test | Status |
|---|---|
| `python3 Scripts/validate_json.py` — JSON, duplikaty, referencje | wykonany: OK, **412 rekordów** |
| `PopiolISol.Progression.Formulas` — XP/HP/obrażenia | napisany |
| `PopiolISol.Quests.Transitions` — start/etap/finał | napisany |
| `PopiolISol.Inventory.Stacks` — stack/remove | napisany |
| `PopiolISol.Quests.FactionBlock` — blokada frakcji | napisany (iteracja 1) |
| `PopiolISol.Chest.Lockpick` — sukces/błąd/wytrych | napisany (iteracja 1) |
| `PopiolISol.Stats.Leveling` — level/LP/HP | napisany (iteracja 1) |

## Smoke test pionowego wycinka (po uruchomieniu PIE)
- **Nowa gra** → gracz pojawia się na `marker_spawn_arrival` z `sword_01`, 3 `lockpick`, 2 `food_bread`.
- **Tutorial/narrative**: podejdź do `Biel` (`npc_neu_01`, marker `marker_bed_npc_neu_01`), naciśnij E — dialog `dialogue_biel_intro` prowadzi do `main_przybysz` (warunek: posiadanie `quest_letter`).
- **Dialog z bezpiecznym wyjściem**: w każdym dialogu klawisz "Dość" zamyka rozmowę bez konsekwencji.
- **Oba łańcuchy kandydackie**:
  - Stary porządek (`Rymar` w Ryglu, `npc_sta_02`): `old_trial_1`..`old_trial_5`. Po ukończeniu — flaga `faction_choice:stary_porzadek`, `new_trial_*` zablokowane.
  - Nowy porządek (`Nita` w Wolnym Brzegu, `npc_now_01`): `new_trial_1`..`new_trial_5`. Po ukończeniu — flaga `faction_choice:nowy_brzeg`, `old_trial_*` zablokowane.
- **Nauczyciel**: `Rymar` pozwala wydać PN na `Sword` (skill rank rośnie, Atak rośnie); `Nita` uczy `Steal`.
- **Miecz/łuk/czar**: zmiana trybu w inspektora, `IA_Attack` wykonuje różne akcje w zależności od trybu.
- **Zamek**: skrzynia (`APISLockedChest`) ma sekwencję `LRL`, 3 `lockpick` w ekwipunku. Po nieudanym ruchu traci się wytrych.
- **Kradzież ze świadkiem**: otwórz skrzynię NPC, jeśli w promieniu 1800 jednostek jest NPC z `CrimeReaction` ≥ `demand`, NPC przechodzi w `Alarm` i atakuje (reputacja spada o 3).
- **Skórowanie**: pokonaj wilczaka (`monster_1`) — first hit → `Downed`, drugi → `Dead` + `DropLoot`. Przy `Skills.Skinning >= 1` SkinMonster dodaje `trophy_01`.
- **Wybór frakcji**: akcja `faction_choice` w ostatnim dialogu ustawia flagę i zamyka drugi łańcuch.
- **Epilog**: tekst końcowy w `main_przybysz` ostatni stage.
- **Save/load**: ESC → Save → Quit → Reload — pozycja, statystyki, ekwipunek, questy i reputacja odtworzone.

## Brakujące automatyzacje P0
- [ ] Test PIE: replay smoke test ścieżki `Biel → Rymar/Nita → faction_choice → epilog → save/load`.
- [ ] Test: zamknięcie NavMesh → NPC używa fallbacku teleportacji.
- [ ] Test: 2 metody rozwiązania questa (walka vs przekupstwo).
- [ ] Test: brakujące ID w dialogu daje log błędu i nie blokuje gry.

## Regresja
- Ważny NPC/quest item nie znika przez walkę/rutynę.
- Zablokowany NavMesh używa fallbacku poza widokiem.
- Nieistniejący ID daje log z plikiem i rekordem.
- `savegame.json` nie jest ładowany jako record (loader pomija).
- Lepszy łańcuch (`old_trial_*`) zakończony → `new_trial_*` odmówione przez `IsFactionBlocked`.
