# Wyzwania i decyzje
- **Pliki `.uasset` nie dają się wygenerować z CLI**: agent tekstowy nie stworzy binarnych blueprintów ani mapy. Rozwiązanie: cała logika runtime jest w C++, a `Scripts/ue_build_project.py` buduje Blueprinty i mapę po stronie edytora (Python Editor Script Plugin). Po jednorazowym uruchomieniu skryptu gracz ma działający PIE.
- **`savegame.json` to dokumentacja schematu**: oba loadery (C++ i CLI) pomijają plik z `documentation_only: true`, żeby nie wciągać przykładu jako rekordu.
- **Niejawne `SetFlag` w dialogach**: parser dialogów musi wywoływać akcje przed oceną `next`, więc flagi ustawione w akcjach wpływają na dalsze węzły w tej samej rozmowie.
- **Blokada frakcji**: heurystyczna — `IsFactionBlocked` sprawdza, czy jakikolwiek quest z przeciwnej frakcji (`old_trial_*` / `new_trial_*`) został ukończony. Ukończenie całego łańcucha faktycznie blokuje drugą ścieżkę.
- **Loot tables**: kontrakt zaktualizowany — `item_id` jest preferowany, `item` zostawiony dla wstecznej kompatybilności. Walidator CLI akceptuje oba warianty.
- **Skórowanie**: implementacja w `APISCharacter::SkinMonster` — wymaga `Skills.Skinning >= 1` (nauczyciel). Upadły (`bDowned`) potwór zamiast ginąć od razu, więc gracz może go ogołocić.
- **Walidator CI**: `Scripts/validate_json.py` w tej wersji akceptuje referencje `item_id` w zagnieżdżonych strukturach (np. `entries[].item_id`). Zweryfikowane: 412 rekordów, 0 błędów.
- **Rutyny NPC a zablokowany NavMesh**: fallback teleportuje NPC poza widok gracza (>1500 jednostek), żeby nigdy nie wchodziły w siebie lub gracza.
- **Falling-back dla unknown actions w dialogach**: nierozpoznane akcje są ignorowane, nie powodują błędu ani nie blokują gry. To zgodne z filarem "wszystko działa nawet w budżecie".
- **Skrypty Python dla edytora**: API UE Python ma kilka niuansów (np. `unreal.Key(name)` zamiast `InputChord`). Skrypt `ue_build_project.py` używa bezpiecznych wywołań i `try/except` per krok, więc częściowy sukces nie blokuje reszty.

## 2026-07-19 — pierwotne decyzje
- Tekstowy agent nie tworzy poprawnego binarnego `.umap` ani nie uruchamia edytora UE; nie udajemy gotowego builda. Projekt kompilowalny i dane są punktem startowym authoringu.
- Nie zastosowano marketplace ani cudzych assetów. Placeholderowe ścieżki assetów są świadomie niegotowe i oznaczone w Art Bible.
- Walidator kontroluje referencje pojedynczych pól `_id`; listy referencji wymagają rozszerzenia kontraktu przy wdrożeniu lootu/questów.
- Audyt 2026-07-19 wykrył niespójne traktowanie dokumentacyjnego `savegame.json` przez loader UE i walidator CLI. Loader pomija teraz rekord z `documentation_only: true`; wynik audytu: `DOCUMENTATION_AUDIT.md`.

- Audyt końcowy wykrył błąd liczbowy w opisie: katalog ma 21, nie 22 questy. Poprawiono README, PROGRESS i raport audytu.
- Assety PNG/OBJ są source art, a nie binarnymi assetami UE; bez Unreal Editor nie można uczciwie deklarować importu, UV, kolizji, rigów ani mapy jako ukończonych.
