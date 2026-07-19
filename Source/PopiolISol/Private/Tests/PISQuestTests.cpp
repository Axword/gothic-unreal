#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "PISQuestComponent.h"
#include "PISInventoryComponent.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPISQuestFlowTest,"PopiolISol.Quests.Transitions",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FPISQuestFlowTest::RunTest(const FString&){auto* Q=NewObject<UPISQuestComponent>();TestTrue(TEXT("Start"),Q->StartQuest(TEXT("main_przybysz"),TEXT("arrival")));TestTrue(TEXT("Stage"),Q->SetStage(TEXT("main_przybysz"),TEXT("choice")));TestTrue(TEXT("Finish"),Q->FinishQuest(TEXT("main_przybysz")));TestEqual(TEXT("Completed"),Q->GetStatus(TEXT("main_przybysz")),EPISQuestStatus::Completed);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPISInventoryTest,"PopiolISol.Inventory.Stacks",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FPISInventoryTest::RunTest(const FString&){auto* I=NewObject<UPISInventoryComponent>();TestTrue(TEXT("Add"),I->AddItem(TEXT("lockpick"),3));TestEqual(TEXT("Count"),I->CountItem(TEXT("lockpick")),3);TestTrue(TEXT("Remove"),I->RemoveItem(TEXT("lockpick"),2));TestEqual(TEXT("Remaining"),I->CountItem(TEXT("lockpick")),1);return true;}
#endif
