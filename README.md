# Popiół i Sól

Oryginalny, polskojęzyczny action RPG 3D dla Unreal Engine 5.4+. Projekt jest **fundamentem produkcyjnym**, nie gotową grywalną wersją: posiada kod C++, katalog danych, testy, ikony i source art, lecz nie zawiera mapy `.umap`, Blueprintów, importowanych `.uasset` ani pakietu Win64.

## Co jest w repozytorium
- JSON jako kanoniczne dane świata: 65 NPC i harmonogramów, 21 questów (1 główny + 5 Rygla + 5 Brzegu + 10 pobocznych), 20 mieczy, 10 łuków, 4 pancerze, 10 roślin, 6 mikstur, 6 stworów i 2 czary.
- Kod fundamentów: loader/raport danych, zegar, formuły progresji, ekwipunek, stan questów, skrzynia L/P oraz pocisk czaru.
- 72 własne ikony PNG, 60 source mesh OBJ/MTL i 5 plansz koncepcyjnych.

## Uruchomienie i weryfikacja
1. Zainstaluj UE 5.4+ oraz Visual Studio 2022 z *Game development with C++*.
2. Wygeneruj pliki projektu z `PopiolISol.uproject`, następnie zbuduj `PopiolISolEditor` (Development Editor, Win64).
3. Uruchom `python3 Scripts/validate_json.py`. Oczekiwany wynik: `validated 243 records; OK`.
4. W UE zaimportuj PNG i OBJ według `Content/Art/SourceModels/README.md`.
5. Utwórz `/Game/Maps/Prototype`, GameMode, postać i Input Mapping Context. Dopiero po tym ustaw mapę jako startową.

## Docelowe sterowanie
WASD — ruch; mysz — kamera; E — interakcja; LPM — atak; PPM — blok/celowanie; I — ekwipunek; J — dziennik; Esc — pauza. Nie jest jeszcze zaimplementowane jako Enhanced Input.

## Status i licencje
Stan realizacji jest w `PROGRESS.md`; bezwzględne braki w `TODO.md`. `THIRD_PARTY_ASSETS.md` rejestruje pochodzenie assetów. Dane kanoniczne są w `Content/Data/Json`; zapis runtime ma trafić do `Saved/SaveGames`, nigdy do `Content`.
