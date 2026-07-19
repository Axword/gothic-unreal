#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "PISProgressionLibrary.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPISProgressionFormulaTest,"PopiolISol.Progression.Formulas",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FPISProgressionFormulaTest::RunTest(const FString&){TestEqual(TEXT("Level 1 XP"),UPISProgressionLibrary::XpForLevel(1),175);TestEqual(TEXT("Base HP"),UPISProgressionLibrary::MaxHpForLevel(1,0),80);TestTrue(TEXT("Sword scales"),UPISProgressionLibrary::SwordDamage(10,10,1)>10);return true;}
#endif
