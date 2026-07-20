// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PISHUD.generated.h"

class UPISHUDWidget;
class UPISInventoryWidget;
class UPISJournalWidget;
class UPISPauseWidget;
class UPISDialogueWidget;
class APISCharacter;

UCLASS()
class POPIOLISOL_API APISHUD : public AHUD
{
    GENERATED_BODY()

public:
    APISHUD();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ToggleInventory();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ToggleJournal();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void TogglePause();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDialogueFor(AActor* Speaker);

    UFUNCTION(BlueprintPure, Category = "HUD")
    bool IsUIOpen() const;

protected:
    UPROPERTY(Transient) TObjectPtr<UPISHUDWidget> HudWidget;
    UPROPERTY(Transient) TObjectPtr<UPISInventoryWidget> InventoryWidget;
    UPROPERTY(Transient) TObjectPtr<UPISJournalWidget> JournalWidget;
    UPROPERTY(Transient) TObjectPtr<UPISPauseWidget> PauseWidget;
    UPROPERTY(Transient) TObjectPtr<UPISDialogueWidget> DialogueWidget;
};
