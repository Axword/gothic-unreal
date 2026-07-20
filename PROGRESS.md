# Postęp
## Ukończone
- Migracja konfiguracji projektu do **Unreal Engine 5.8** (uproject, Target.cs V7/Unreal5_8, mapy startowe silnika, fix kompilacji pocisku).
- Struktura projektu, dokumentacja, dane kanoniczne JSON i niezależna walidacja.
- 65 nazwanych NPC ze slotami praca/jedzenie/patrol/sen; bazowy zegar C++.
- C++: ID-only inventory, quest state machine, skrzynia z sekwencyjnym zamkiem i pocisk czaru.
- Audyt dokumentacji i minimów danych; poprawiono obsługę dokumentacyjnego savegame w loaderze UE.
- 72 własne źródłowe ikony PNG i 5 plansz koncepcyjnych, z pełnym wpisem licencyjnym.
- 60 własnych modeli źródłowych OBJ/MTL dla wyposażenia, stworów i świata; wymagają importu oraz finalnego setupu UE.
- Wymagane minima katalogowe (20 mieczy, 10 łuków, 4 pancerze, 10 roślin, 6 mikstur, 6 potworów, 21 questów (1 główny + 5 + 5 + 10)).
## W toku / nieuznane za ukończone
- Mapa Prototype, postać, Enhanced Input, UI, AI, save/load, import/setup meshów, rigging, animacje i audio.
- Ikony PNG istnieją, ale trzeba je zaimportować jako Texture2D i związać z UMG w UE Editor.
- Komponenty C++ wymagają osadzenia na postaci/Blueprintach w edytorze.
## Następny krok
P0: stworzyć mapę i blueprintowy vertical slice: 3 NPC, chest/lock, wilczak, miecz, Żarowy pocisk oraz `main_przybysz`.
- Katalogi broni, pancerzy, roślin i mikstur mają teraz podstawowe pola gameplayowe; 21 questów ma opisane cele, gałęzie, porażki, dziennik i nagrody.
- Dostępne są trzy przykładowe dialogi JSON (Biel, Rymar, Nita), gotowe pod interpreter runtime.
