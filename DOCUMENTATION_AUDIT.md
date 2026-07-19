# Audyt dokumentacji — 2026-07-19

## Wynik
Wymagane 13 plików Markdown istnieje. Nazwy, ścieżki danych i deklarowane liczebności zostały porównane z repozytorium.

| Kontrola | Wynik |
|---|---|
| Wymagane dokumenty | OK — 13/13 |
| Walidator JSON | OK — 243 rekordy |
| NPC / harmonogramy | OK — 65 / 65 |
| Miecze / łuki / pancerze | OK — 20 / 10 / 4 |
| Rośliny / mikstury / stwory / czary | OK — 10 / 6 / 6 / 2 |
| Questy stare / nowe / poboczne / główne | OK — 5 / 5 / 10 / 1 |
| Binarna mapa, Blueprinty, assety, build | Nie istnieją — poprawnie zaznaczone jako P0 lub praca w toku |

## Znaleziona i naprawiona rozbieżność
`savegame.json` jest przykładem dokumentacji schematu, a nie plikiem z `records`. Walidator Python już go pomijał, ale loader UE wcześniej próbował go interpretować jak dane kanoniczne. Loader C++ teraz rozpoznaje `documentation_only: true` i pomija taki plik. Dzięki temu raport runtime i walidator CLI mają tę samą semantykę.

## Świadome ograniczenia
README nie obiecuje mapy ani builda. `DefaultEngine.ini` wskazuje planowaną mapę `/Game/Maps/Prototype`; dopóki mapa nie zostanie utworzona w UE Editor, projekt nie jest uruchamialnym vertical slice. Pozostaje to najważniejszym P0, a nie błąd dokumentacji.
