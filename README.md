# Popiół i Sól

Oryginalny, polskojęzyczny action RPG 3D dla **Unreal Engine 5.8**. Projekt jest **fundamentem produkcyjnym**, nie gotową grywalną wersją: posiada kod C++, katalog danych, testy, ikony i source art, lecz nie zawiera mapy `.umap`, Blueprintów, importowanych `.uasset` ani pakietu Win64.

## Co jest w repozytorium
- JSON jako kanoniczne dane świata: 65 NPC i harmonogramów, 21 questów (1 główny + 5 Rygla + 5 Brzegu + 10 pobocznych), 20 mieczy, 10 łuków, 4 pancerze, 10 roślin, 6 mikstur, 6 stworów i 2 czary.
- Kod fundamentów: loader/raport danych, zegar, formuły progresji, ekwipunek, stan questów, skrzynia L/P oraz pocisk czaru.
- 72 własne ikony PNG, 60 source mesh OBJ/MTL i 5 plansz koncepcyjnych.

## Wymagania (UE 5.8)
- **Unreal Engine 5.8** (Epic Launcher lub build ze źródeł).
- **Visual Studio 2022 lub 2026** z workloadem *Desktop development with C++* / *Game development with C++*.
- Preferowany toolset MSVC wskazany przez UE 5.8 (ostrzeżenie o `14.50+` / v145 — zainstaluj zalecany komponent).
- Redistributable z `UE_5.8/Engine/Extras/Redist/en-us/vc_redist.x64.exe`, jeśli edytor nie startuje po kompilacji.

## Uruchomienie i weryfikacja
1. Upewnij się, że `PopiolISol.uproject` ma `EngineAssociation: "5.8"`. Jeśli launcher nie widzi 5.8, *Switch Unreal Engine version…* → wskaż folder instalacji ręcznie.
2. Usuń stare cache po migracji z 5.4 (jeśli istnieją): `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, `*.sln`.
3. PPM na `.uproject` → *Generate Visual Studio project files*, potem zbuduj `PopiolISolEditor` (Development Editor, Win64).
4. Uruchom `python3 Scripts/validate_json.py`. Oczekiwany wynik: `validated … records; OK`.
5. W UE zaimportuj PNG i OBJ według `Content/Art/SourceModels/README.md`.
6. Utwórz `/Game/Maps/Prototype`, GameMode, postać i Input Mapping Context. Dopiero potem ustaw mapę jako startową w *Project Settings → Maps & Modes* (albo w `Config/DefaultEngine.ini`).

### Target.cs (już ustawione pod 5.8)
```
DefaultBuildSettings = BuildSettingsVersion.V7;
IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
```

## Docelowe sterowanie
WASD — ruch; mysz — kamera; E — interakcja; LPM — atak; PPM — blok/celowanie; I — ekwipunek; J — dziennik; Esc — pauza. Nie jest jeszcze zaimplementowane jako Enhanced Input w postaci gracza (plugin jest włączony).

## Status i licencje
Stan realizacji jest w `PROGRESS.md`; bezwzględne braki w `TODO.md`. `THIRD_PARTY_ASSETS.md` rejestruje pochodzenie assetów. Dane kanoniczne są w `Content/Data/Json`; zapis runtime ma trafić do `Saved/SaveGames`, nigdy do `Content`.
