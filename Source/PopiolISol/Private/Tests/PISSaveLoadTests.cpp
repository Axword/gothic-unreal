// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "PISStatsComponent.h"
#include "PISInventoryComponent.h"
#include "PISQuestComponent.h"
#include "PISLockedChest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FPISFactionBlockTest,
    "PopiolISol.Quests.FactionBlock",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPISFactionBlockTest::RunTest(const FString& /*Parameters*/)
{
    UPISQuestComponent* Quest = NewObject<UPISQuestComponent>();
    Quest->StartQuest(TEXT("old_trial_1"), TEXT("start"));
    Quest->FinishQuest(TEXT("old_trial_1"), true);
    TestFalse(TEXT("New faction locked after old trial finished"),
        Quest->StartQuest(TEXT("new_trial_1"), TEXT("start")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FPISLockpickTest,
    "PopiolISol.Chest.Lockpick",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPISLockpickTest::RunTest(const FString& /*Parameters*/)
{
    APISLockedChest* Chest = NewObject<APISLockedChest>();
    Chest->Sequence = TEXT("RLR");
    UPISInventoryComponent* Inv = NewObject<UPISInventoryComponent>();
    Inv->AddItem(TEXT("lockpick"), 3);
    TestTrue(TEXT("Begin"), Chest->BeginLockpick(1));
    TestFalse(TEXT("Right 1"), Chest->SubmitLockTurn(true, Inv));
    TestFalse(TEXT("Left"),  Chest->SubmitLockTurn(false, Inv));
    TestFalse(TEXT("Right 2"), Chest->SubmitLockTurn(true, Inv));
    TestTrue(TEXT("Right 3"), Chest->SubmitLockTurn(true, Inv));
    TestTrue(TEXT("Opened"), Chest->bOpened);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FPISProgressionLevelTest,
    "PopiolISol.Stats.Leveling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPISProgressionLevelTest::RunTest(const FString& /*Parameters*/)
{
    UPISStatsComponent* Stats = NewObject<UPISStatsComponent>();
    const int32 XpFor2 = Stats->GetXpForNextLevel();
    Stats->AwardXp(XpFor2);
    TestEqual(TEXT("Level 2"), Stats->Level, 2);
    TestEqual(TEXT("LP gained"), Stats->LearningPoints, 10);
    TestEqual(TEXT("MaxHP scales"), Stats->GetMaxHp(), 80 + 12 + 5 * 4);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
