# Third-party assets
Brak. Obecna iteracja nie zawiera assetów zewnętrznych ani audio. Przed każdym dodaniem wpis wymagany: URL, autor, licencja, plik, modyfikacja i przeznaczenie.

## Własne assety iteracji 0.2
- `Content/Art/Icons/icon_*.png` — 72 oryginalne, proceduralnie narysowane ikony PNG 128×128. Autor: projekt Popiół i Sól. Licencja: własność projektu. Generator: `Scripts/generate_placeholder_icons.py`. Są to gotowe pliki źródłowe do importu jako `Texture2D` w UE; po imporcie odpowiadają ścieżkom `/Game/Art/Icons/icon_<id>` w JSON.
- `Content/Art/Concepts/*.png` — 5 obrazów koncepcyjnych wygenerowanych na potrzeby tego projektu: osady, stwory, pancerze, broń i kierunek ikon. Autor/źródło: generator obrazu Arena.ai, 2026-07-19. Użycie: wewnętrzna referencja produkcyjna, nie finalny mesh, tekstura ani materiał gry.
- `Content/Art/SourceModels/*.obj` i `pis_materials.mtl` — 60 oryginalnych, proceduralnie wygenerowanych low-poly modeli źródłowych. Autor: projekt Popiół i Sól. Licencja: własność projektu. Generator: `Scripts/generate_source_models.py`. Nie zawierają geometrii osób trzecich.
