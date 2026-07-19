# Changelog
## 2026-07-19 — Iteration 0
- Utworzono projekt UE 5.4 C++ i fundament JSON.
- Dodano 243 rekordy: 65 nazwanych NPC i harmonogramy, katalog przedmiotów, 6 stworów, questy, lokacje, trenerów i czary.
- Dodano loader runtime, walidator, zegar świata i test formuł.
- Zdefiniowano oryginalny świat Popiołu i Soli.

## 2026-07-19 — Iteration 0.1
- Dodano komponenty ekwipunku i przejść questów, z eventem dla UMG/Blueprintów.
- Dodano aktora skrzyni z trzyruchowym zamkiem L/P i kosztem wytrycha oraz pocisk czaru z obrażeniami.
- Dodano testy automatyczne inventory i przepływu questa.

## 2026-07-19 — Dokumentation audit
- Sprawdzono komplet obowiązkowych Markdownów i zgodność deklarowanych minimów z JSON.
- Loader C++ pomija teraz dokumentacyjny `savegame.json`, zgodnie z walidatorem CLI i opisem schematu.

## 2026-07-19 — Iteration 0.2: źródła wizualne
- Dodano 72 własne, proceduralne ikony PNG 128×128 oraz wersjonowany generator bez zależności zewnętrznych.
- Dodano 5 plansz koncepcyjnych dla świata, stworów, pancerzy, broni i UI.
- Ścieżki assetów w katalogach JSON wskazują teraz importowalne ikony `/Game/Art/Icons/icon_<id>`.

## 2026-07-19 — Iteration 0.3: modele źródłowe
- Dodano 60 własnych modeli OBJ low-poly i MTL: bronie, pancerze, stwory, rośliny oraz propsy świata.
- Dodano generator geometrii i instrukcję importu do UE; JSON zawiera ścieżki source mesh dla odpowiednich rekordów.

## 2026-07-19 — Audyt zakresu i dokumentacji
- Rozszerzono README, DATA_SCHEMAS i raport audytu o faktyczny stan importu, danych oraz znane ograniczenia.
- Skorygowano liczbę questów: 21 (1 + 5 + 5 + 10), nie 22.

## 2026-07-19 — Iteration 0.4: dane grywalnego przepływu
- Rozbudowano katalog przedmiotów o statystyki i wymagania oraz 21 questów o cele, dziennik, gałęzie, porażki i nagrody.
- Dodano trzy dialogi z bezpiecznym wyjściem, warunkami i akcjami startu questów.
- Walidator CLI rekurencyjnie sprawdza teraz zagnieżdżone referencje `item_id` i `quest_id`.
