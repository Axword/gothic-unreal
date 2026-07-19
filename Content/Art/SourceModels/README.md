# Source models — import do Unreal Engine

To są oryginalne, niskopoligonowe modele źródłowe OBJ, wygenerowane przez `Scripts/generate_source_models.py`; nie są plikami `.uasset`. W UE Editor wybierz **Import** w folderze docelowym `/Game/Art/Meshes/`, zaznacz odpowiednie `.obj`, importuj materiały z `pis_materials.mtl` lub utwórz materiały UE o tych samych nazwach. Następnie ustaw LOD, collision i przypisz finalne ścieżki `StaticMesh` w Blueprintach/data runtime.

Zestaw: 20 mieczy, 10 łuków, 4 pancerze, 6 stworów, 10 roślin oraz 10 modułów/propów środowiska. Modele są prostymi placeholderami produkcyjnymi: mają czytelny zarys i materiały bazowe, ale nie zastępują finalnego riggingu, UV, LOD, kolizji ani animacji.
