// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISHUD.h"
#include "PISHUDWidget.h"
#include "PISInventoryWidget.h"
#include "PISJournalWidget.h"
#include "PISPauseWidget.h"
#include "PISDialogueWidget.h"
#include "PISCharacter.h"
#include "PISDialogueComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

APISHUD::APISHUD()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APISHUD::BeginPlay()
{
    Super::BeginPlay();
    if (!HudWidget)
    {
        HudWidget = CreateWidget<UPISHUDWidget>(GetWorld(), UPISHUDWidget::StaticClass());
        if (HudWidget) { HudWidget->AddToViewport(0); }
    }
}

void APISHUD::EndPlay(const EEndPlayReason::Type Reason)
{
    if (HudWidget)      { HudWidget->RemoveFromParent();      HudWidget = nullptr; }
    if (InventoryWidget){ InventoryWidget->RemoveFromParent(); InventoryWidget = nullptr; }
    if (JournalWidget)  { JournalWidget->RemoveFromParent();   JournalWidget = nullptr; }
    if (PauseWidget)    { PauseWidget->RemoveFromParent();     PauseWidget = nullptr; }
    if (DialogueWidget) { DialogueWidget->RemoveFromParent();  DialogueWidget = nullptr; }
    Super::EndPlay(Reason);
}

bool APISHUD::IsUIOpen() const
{
    return (InventoryWidget && InventoryWidget->IsInViewport())
        || (JournalWidget   && JournalWidget->IsInViewport())
        || (PauseWidget     && PauseWidget->IsInViewport())
        || (DialogueWidget  && DialogueWidget->IsInViewport());
}

void APISHUD::ToggleInventory()
{
    if (InventoryWidget && InventoryWidget->IsInViewport())
    {
        InventoryWidget->RemoveFromParent();
        if (APlayerController* PC = GetOwningPlayerController()) { PC->bShowMouseCursor = false; }
        return;
    }
    if (JournalWidget)  { JournalWidget->RemoveFromParent(); }
    if (PauseWidget)    { PauseWidget->RemoveFromParent(); }
    if (DialogueWidget) { DialogueWidget->RemoveFromParent(); }
    if (!InventoryWidget)
    {
        InventoryWidget = CreateWidget<UPISInventoryWidget>(GetWorld(), UPISInventoryWidget::StaticClass());
    }
    if (InventoryWidget)
    {
        InventoryWidget->AddToViewport(10);
        InventoryWidget->Refresh();
        if (APlayerController* PC = GetOwningPlayerController()) { PC->bShowMouseCursor = true; }
    }
}

void APISHUD::ToggleJournal()
{
    if (JournalWidget && JournalWidget->IsInViewport())
    {
        JournalWidget->RemoveFromParent();
        if (APlayerController* PC = GetOwningPlayerController()) { PC->bShowMouseCursor = false; }
        return;
    }
    if (InventoryWidget){ InventoryWidget->RemoveFromParent(); }
    if (PauseWidget)    { PauseWidget->RemoveFromParent(); }
    if (DialogueWidget) { DialogueWidget->RemoveFromParent(); }
    if (!JournalWidget)
    {
        JournalWidget = CreateWidget<UPISJournalWidget>(GetWorld(), UPISJournalWidget::StaticClass());
    }
    if (JournalWidget)
    {
        JournalWidget->AddToViewport(10);
        JournalWidget->Refresh();
        if (APlayerController* PC = GetOwningPlayerController()) { PC->bShowMouseCursor = true; }
    }
}

void APISHUD::TogglePause()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) { return; }
    if (PauseWidget && PauseWidget->IsInViewport())
    {
        PauseWidget->RemoveFromParent();
        PC->SetPause(false);
        PC->bShowMouseCursor = false;
        return;
    }
    if (InventoryWidget){ InventoryWidget->RemoveFromParent(); }
    if (JournalWidget)  { JournalWidget->RemoveFromParent(); }
    if (DialogueWidget) { DialogueWidget->RemoveFromParent(); }
    if (!PauseWidget)
    {
        PauseWidget = CreateWidget<UPISPauseWidget>(GetWorld(), UPISPauseWidget::StaticClass());
    }
    if (PauseWidget)
    {
        PauseWidget->AddToViewport(20);
        PC->SetPause(true);
        PC->bShowMouseCursor = true;
    }
}

void APISHUD::ShowDialogueFor(AActor* Speaker)
{
    APISCharacter* Player = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Player) { return; }
    UPISDialogueComponent* Dlg = Player->FindComponentByClass<UPISDialogueComponent>();
    if (!Dlg) { return; }
    if (InventoryWidget){ InventoryWidget->RemoveFromParent(); }
    if (JournalWidget)  { JournalWidget->RemoveFromParent(); }
    if (PauseWidget)    { PauseWidget->RemoveFromParent(); }
    if (!DialogueWidget)
    {
        DialogueWidget = CreateWidget<UPISDialogueWidget>(GetWorld(), UPISDialogueWidget::StaticClass());
    }
    if (DialogueWidget)
    {
        if (!DialogueWidget->IsInViewport())
        {
            DialogueWidget->AddToViewport(15);
        }
        DialogueWidget->BindDialogue(Dlg);
        // Show the current node immediately (OpenDialogue already fired it
        // before the widget was created, so we manually push the cached view).
        if (Dlg->IsOpen())
        {
            // Forward-declare access through the widget.
            DialogueWidget->OnNodeShown(Dlg->GetCurrentNode());
        }
    }
}
