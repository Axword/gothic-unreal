#include "MockUECore.h"

// Override CoreMinimal / headers for mock compile
#define CoreMinimal_h "MockUECore.h"
#define Components_ActorComponent_h "MockUECore.h"
#define GameFramework_Actor_h "MockUECore.h"
#define Kismet_BlueprintFunctionLibrary_h "MockUECore.h"
#define Components_BoxComponent_h "MockUECore.h"
#define Components_StaticMeshComponent_h "MockUECore.h"

#include "../Source/PopiolISol/Public/PISProgressionLibrary.h"
#include "../Source/PopiolISol/Public/PISInventoryComponent.h"
#include "../Source/PopiolISol/Public/PISQuestComponent.h"
#include "../Source/PopiolISol/Public/PISLockedChest.h"
#include "../Source/PopiolISol/Public/PISWorldClock.h"

#include "../Source/PopiolISol/Private/PISInventoryComponent.cpp"
#include "../Source/PopiolISol/Private/PISQuestComponent.cpp"
#include "../Source/PopiolISol/Private/PISLockedChest.cpp"
#include "../Source/PopiolISol/Private/PISWorldClock.cpp"

int main() {
    int passed = 0;
    int failed = 0;

    auto check = [&](const char* test_name, bool cond) {
        if (cond) {
            std::cout << "[PASS] " << test_name << "\n";
            passed++;
        } else {
            std::cerr << "[FAIL] " << test_name << "\n";
            failed++;
        }
    };

    std::cout << "=== Running Headless C++ Logic Test Harness ===\n";

    // 1. Progression Library Formulas
    check("Progression.XpForLevel(1)", UPISProgressionLibrary::XpForLevel(1) == 175);
    check("Progression.MaxHpForLevel(1, 0)", UPISProgressionLibrary::MaxHpForLevel(1, 0) == 80);
    check("Progression.SwordDamage(10, 10, 1)", UPISProgressionLibrary::SwordDamage(10, 10, 1) == 19.5f);
    check("Progression.BowDamage(15, 12, 1)", UPISProgressionLibrary::BowDamage(15, 12, 1) == 25.2f);

    // 2. Inventory Component
    UPISInventoryComponent Inv;
    check("Inventory.AddItem lockpick x3", Inv.AddItem("lockpick", 3));
    check("Inventory.CountItem lockpick == 3", Inv.CountItem("lockpick") == 3);
    check("Inventory.RemoveItem lockpick x2", Inv.RemoveItem("lockpick", 2));
    check("Inventory.CountItem lockpick == 1", Inv.CountItem("lockpick") == 1);
    Inv.AddItem("sword_01", 1);
    Inv.AddItem("sword_02", 1);
    check("Inventory.EquipWeapon succeeds with enough count", Inv.EquipWeapon("sword_01"));
    check("Inventory.EquipWeaponWithStats succeeds when strength enough", Inv.EquipWeaponWithStats("sword_02", 20, 10, 15, 0));
    check("Inventory.EquipWeaponWithStats fails when strength low", !Inv.EquipWeaponWithStats("sword_05", 10, 10, 50, 0));

    // 3. Quest Component (Flow, Idempotency, and Faction Mutual Exclusion)
    UPISQuestComponent Quest;
    check("Quest.StartQuest main_przybysz", Quest.StartQuest("main_przybysz", "arrival"));
    check("Quest.SetStage main_przybysz -> choice", Quest.SetStage("main_przybysz", "choice"));
    check("Quest.FinishQuest main_przybysz succeeds", Quest.FinishQuest("main_przybysz", true));
    check("Quest.GetStatus main_przybysz == Completed", Quest.GetStatus("main_przybysz") == EPISQuestStatus::Completed);
    check("Quest.FinishQuest main_przybysz again fails (idempotency)", !Quest.FinishQuest("main_przybysz", true));

    check("Quest.StartQuest old_trial_1", Quest.StartQuest("old_trial_1", "start"));
    check("Quest.ChooseFaction stary_porzadek", Quest.ChooseFaction("stary_porzadek"));
    check("Quest.ChooseFaction again fails", !Quest.ChooseFaction("nowy_porzadek"));
    check("Quest.IsQuestLocked new_trial_1 == true", Quest.IsQuestLocked("new_trial_1"));
    check("Quest.StartQuest on locked new_trial_1 fails", !Quest.StartQuest("new_trial_1", "start"));

    // 4. Locked Chest (Pick Consumption and Sequence)
    APISLockedChest Chest;
    UPISInventoryComponent EmptyInv;
    check("Chest.BeginLockpickWithInventory fails when 0 picks", !Chest.BeginLockpickWithInventory(1, &EmptyInv));
    Inv.AddItem("lockpick", 2); // now count is 1 + 2 = 3
    check("Chest.BeginLockpickWithInventory succeeds with picks", Chest.BeginLockpickWithInventory(1, &Inv));
    check("Chest.SubmitLockTurn right (R) when expecting L fails & consumes pick", !Chest.SubmitLockTurn(true, &Inv));
    check("Chest pick count reduced to 2", Inv.CountItem("lockpick") == 2);
    check("Chest.SubmitLockTurn left (L) succeeds", Chest.SubmitLockTurn(false, &Inv));
    check("Chest.SubmitLockTurn right (R) succeeds", Chest.SubmitLockTurn(true, &Inv));
    check("Chest.SubmitLockTurn left (L) completes lock", Chest.SubmitLockTurn(false, &Inv));
    check("Chest is opened", Chest.bOpened);

    // 5. World Clock (Sleep across midnight)
    APISWorldClock Clock;
    Clock.Hour = 22.f;
    Clock.Day = 1;
    Clock.SleepTo(6.f);
    check("Clock.SleepTo(6.f) from 22.f increments Day", Clock.Day == 2 && Clock.Hour == 6.f);

    std::cout << "=== Headless C++ Harness Summary: " << passed << " passed, " << failed << " failed ===\n";
    return failed == 0 ? 0 : 1;
}
