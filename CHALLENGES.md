# Wyzwania i decyzje
- Tekstowy agent nie tworzy poprawnego binarnego `.umap` ani nie uruchamia edytora UE; nie udajemy gotowego builda. Projekt kompilowalny i dane są punktem startowym authoringu.
- Nie zastosowano marketplace ani cudzych assetów. Placeholderowe ścieżki assetów są świadomie niegotowe i oznaczone w Art Bible.
- Walidator kontroluje referencje pojedynczych pól `_id`; listy referencji wymagają rozszerzenia kontraktu przy wdrożeniu lootu/questów.
- Audyt 2026-07-19 wykrył niespójne traktowanie dokumentacyjnego `savegame.json` przez loader UE i walidator CLI. Loader pomija teraz rekord z `documentation_only: true`; wynik audytu: `DOCUMENTATION_AUDIT.md`.

- Audyt końcowy wykrył błąd liczbowy w opisie: katalog ma 21, nie 22 questy. Poprawiono README, PROGRESS i raport audytu.
- Assety PNG/OBJ są source art, a nie binarnymi assetami UE; bez Unreal Editor nie można uczciwie deklarować importu, UV, kolizji, rigów ani mapy jako ukończonych.
