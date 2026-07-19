# Wyzwania i decyzje
- Tekstowy agent nie tworzy poprawnego binarnego `.umap` ani nie uruchamia edytora UE; nie udajemy gotowego builda. Projekt kompilowalny i dane są punktem startowym authoringu.
- Nie zastosowano marketplace ani cudzych assetów. Placeholderowe ścieżki assetów są świadomie niegotowe i oznaczone w Art Bible.
- [Agent nr 2] Walidator Python (`validate_json.py`) oraz C++ (`UPISJsonDataSubsystem.cpp`) zostały rozszerzone o pełną rekurencyjną weryfikację referencji zagnieżdżonych w listach i obiektach (`slots`, `objectives`, `entries`, `nodes`, `choices`, akcje `start:`, `set_stage:`, `finish:`) oraz kontrolę wymagań domenowych.
- [Agent nr 2] Do weryfikacji logiki C++ w środowisku bez binarnego edytora UE zbudowano lokalny harness testowy `Scripts/run_cpp_harness.cpp` (kompilowany `g++ -std=c++20`) z symulacją podstawowych struktur silnikowych oraz kompletny test integracyjny `Scripts/smoke_test.py` weryfikujący 18 kroków vertical slice'a.
