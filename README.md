# Popiół i Sól

Oryginalny, polskojęzyczny action-RPG 3D dla UE 5.4+. Repozytorium zawiera **Iteration 0: działający fundament danych i vertical-slice authoring**; nie zawiera binarnego builda ani gotowej mapy `.umap` (te powstają w edytorze UE i nie mogą być wiarygodnie wygenerowane jako tekst).

## Uruchomienie
1. Zainstaluj Unreal Engine 5.4+ i Visual Studio 2022 z workloadem *Game development with C++*.
2. Prawym przyciskiem na `PopiolISol.uproject` → **Generate Visual Studio project files**, potem zbuduj target `PopiolISolEditor` w Development Editor/Win64.
3. Otwórz projekt. Utwórz `/Game/Maps/Prototype`, ustaw go jako mapę startową, dodaj NavMeshBoundsVolume, Directional Light, SkyLight i BP pochodny od `APISWorldClock`.
4. Zweryfikuj dane: `python Scripts/validate_json.py`; raport runtime jest dostępny z `UPISJsonDataSubsystem.GetValidationReport`.

Sterowanie docelowe: WASD/ruch, mysz/kamera, E/interakcja, LPM/atak, PPM/blok/celowanie, I/ekwipunek, J/dziennik, Esc/pauza. Mapowanie Enhanced Input i widgety są P0 następnej iteracji.

Nie użyto obcych assetów. Wszystkie dane kanoniczne są w `Content/Data/Json`; stan zapisu ma być zapisywany poza Content.
