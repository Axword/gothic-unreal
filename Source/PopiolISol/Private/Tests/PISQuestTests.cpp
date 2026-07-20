// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "PISQuestComponent.h"
#include "PISInventoryComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPISQuestFlowTest,
	"PopiolISol.Quests.Transitions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPISQuestFlowTest::RunTest(const FString& /*Parameters*/)
{
	UPISQuestComponent* Quest = NewObject<UPISQuestComponent>();
	TestTrue(TEXT("Start"), Quest->StartQuest(TEXT("main_przybysz"), TEXT("arrival")));
	TestTrue(TEXT("Stage"), Quest->SetStage(TEXT("main_przybysz"), TEXT("choice")));
	TestTrue(TEXT("Finish"), Quest->FinishQuest(TEXT("main_przybysz")));
	TestEqual(TEXT("Completed"), Quest->GetStatus(TEXT("main_przybysz")), EPISQuestStatus::Completed);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPISInventoryTest,
	"PopiolISol.Inventory.Stacks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPISInventoryTest::RunTest(const FString& /*Parameters*/)
{
	UPISInventoryComponent* Inventory = NewObject<UPISInventoryComponent>();
	TestTrue(TEXT("Add"), Inventory->AddItem(TEXT("lockpick"), 3));
	TestEqual(TEXT("Count"), Inventory->CountItem(TEXT("lockpick")), 3);
	TestTrue(TEXT("Remove"), Inventory->RemoveItem(TEXT("lockpick"), 2));
	TestEqual(TEXT("Remaining"), Inventory->CountItem(TEXT("lockpick")), 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
