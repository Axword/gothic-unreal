# Architektura
`UPISJsonDataSubsystem` ładuje `Content/Data/Json` przy starcie, wykrywa duplikaty oraz referencje `*_id`, logując plik/ID. Pythonowy walidator jest szybkim testem CI. JSON jest źródłem prawdy; Blueprinty są tylko prezentacją i kompozycją. `APISWorldClock` dostarcza czas/dzień/noc. `UPISProgressionLibrary` zawiera czyste, testowalne formuły.

Plan runtime: GameInstance (dane/save) → GameMode (spawn) → Character components (inventory, combat, quests) → AI controller/schedule service → UMG. Save JSON posiada wersję, stabilne ID i wartości domyślne; nigdy wskaźniki UE.
