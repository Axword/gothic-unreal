#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "PISProgressionLibrary.h"
#include "PISWorldClock.h"
#include "PISLockedChest.h"
#include "PISInventoryComponent.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPISProgressionFormulaTest,"PopiolISol.Progression.Formulas",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FPISProgressionFormulaTest::RunTest(const FString&){TestEqual(TEXT("Level 1 XP"),UPISProgressionLibrary::XpForLevel(1),175);TestEqual(TEXT("Base HP"),UPISProgressionLibrary::MaxHpForLevel(1,0),80);TestTrue(TEXT("Sword scales"),UPISProgressionLibrary::SwordDamage(10,10,1)>10);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPISClockSleepTest,"PopiolISol.Clock.SleepAcrossMidnight",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FPISClockSleepTest::RunTest(const FString&){
 auto* C=NewObject<APISWorldClock>();
 C->Hour=22.f;C->Day=1;
 C->SleepTo(6.f);
 TestEqual(TEXT("Day incremented"),C->Day,2);
 TestEqual(TEXT("Hour set"),C->Hour,6.f);
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPISLockedChestTest,"PopiolISol.Mechanics.LockedChestPickConsumption",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FPISLockedChestTest::RunTest(const FString&){
 auto* Chest=NewObject<APISLockedChest>();
 auto* Inv=NewObject<UPISInventoryComponent>();
 TestFalse(TEXT("Cannot pick without lockpicks"),Chest->BeginLockpickWithInventory(1,Inv));
 Inv->AddItem(TEXT("lockpick"),1);
 TestTrue(TEXT("Begin pick with pick"),Chest->BeginLockpickWithInventory(1,Inv));
 TestFalse(TEXT("Submit wrong turn consumes pick"),Chest->SubmitLockTurn(false,Inv));
 TestEqual(TEXT("0 lockpicks remaining"),Inv->CountItem(TEXT("lockpick")),0);
 return true;
}
#endif
