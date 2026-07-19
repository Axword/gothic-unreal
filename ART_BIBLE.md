# Art bible
Low/mid-poly, ręcznie stylizowane i surowe: grafit `#25272A`, mokra zieleń `#3D5042`, sól `#B6B1A0`, rdza `#744331`, akcent magiczny `#78A7B5`. Mocne niskie światło kierunkowe i głębokie cienie; czytelne sylwetki. Rygiel: proste piony, płaszcze i emblemat rygla. Wolny Brzeg: skośne łaty, lina, ochra. Dostarczono source art PNG/OBJ; po imporcie własne meshe muszą zostać skonfigurowane jako `.uasset`. Nadal brakuje finalnych materiałów UE, rigów, animacji i VFX. Budżet: 2k tris postać, 1k potwór, 500 prop; instancing roślin, LOD/HLOD.

## Dostarczone źródła wizualne (iteracja 0.2)
`Content/Art/Icons/` zawiera 72 importowalne PNG 128×128 dla katalogu broni, zbroi, roślin, mikstur, trofeów, przedmiotów użytkowych, czarów i potworów. Ikony są celowo proste, lecz mają własne kolory kategorii i rozpoznawalne piktogramy; generator jest wersjonowany. `Content/Art/Concepts/` ma pięć plansz referencyjnych. To **nie** są jeszcze modele 3D, materiały, animacje ani finalne UI atlas textures.

## Modele źródłowe (iteracja 0.3)
`Content/Art/SourceModels/` dostarcza 60 własnych, niskopoligonowych plików OBJ i wspólny `pis_materials.mtl`: 20 mieczy, 10 łuków, 4 pancerze, 6 stworów, 10 roślin i 10 propsów/modułów. Instrukcja importu znajduje się w `Content/Art/SourceModels/README.md`. Są to rzeczywiste źródła geometrii, jednak nie są jeszcze `.uasset`; wymagają importu, ustawienia UV/lightmap, kolizji, LOD i rigowania/animacji w UE.
