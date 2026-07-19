# Postęp
## Ukończone
- Struktura projektu, dokumentacja, dane kanoniczne JSON i niezależna walidacja.
- 65 nazwanych NPC ze slotami praca/jedzenie/patrol/sen; bazowy zegar C++.
- C++: ID-only inventory, quest state machine, skrzynia z sekwencyjnym zamkiem i pocisk czaru.
- Wymagane minima katalogowe (20 mieczy, 10 łuków, 4 pancerze, 10 roślin, 6 mikstur, 6 potworów, 22 questy).
- [Agent nr 2 - Audyt] Utworzono dokumentację audytu, zidentyfikowano błędy statyczne i rozpoczęto pętlę naprawczą.
## W toku / nieuznane za ukończone
- Rozbudowa walidatora JSON i C++ o zagnieżdżone referencje i sprawdzanie schematów domeny.
- Uzupełnienie atrybutów bojowych w broniach, zbrojach, potworach i czarach.
- Usunięcie atrap z zadań kandydackich i pobocznych oraz stworzenie pełnej bazy dialogowej dla 65 NPC i wyboru frakcji.
- Naprawa mechanik C++ (`PISQuestComponent`, `PISLockedChest`, `PISWorldClock`, `PISInventoryComponent`).
- Mapa Prototype, postać, Enhanced Input, UI, AI, save/load i assety (wymagają edytora UE / UMG).
## Następny krok
Naprawa P1: walidator JSON/C++, statystyki bojowe w JSON, rozbudowa dialogów i questów, naprawa logiki C++ i testy automatyczne.
