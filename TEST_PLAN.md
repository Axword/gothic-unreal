# Test plan
Automatyczne: `python Scripts/validate_json.py`; UE Automation `PopiolISol.Progression.Formulas`. Do dodać: loader invalid/missing ref, XP/damage, quest transitions, save migration/round trip.

Smoke po wdrożeniu: nowa gra → tutorial → dialog/branch → oba łańcuchy kandydackie → nauczyciel → miecz/łuk/czar → zamek → kradzież ze świadkiem → skórowanie → wybór → epilog → save/load. Regresja: ważny NPC/quest item nie znika przez walkę/rutynę, zablokowany NavMesh używa fallbacku poza widokiem, nie ma soft-locka dialogu.
