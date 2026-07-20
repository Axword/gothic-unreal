// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PISDialogueComponent.h"
#include "PISDialogueWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UButton;
class UBorder;
class APISCharacter;
class UPISDialogueComponent;

UCLASS()
class POPIOLISOL_API UPISDialogueWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnNodeShown(const FPISDialogueNodeView& Node);

    UFUNCTION() void OnChoice0() { Choose(0); }
    UFUNCTION() void OnChoice1() { Choose(1); }
    UFUNCTION() void OnChoice2() { Choose(2); }
    UFUNCTION() void OnChoice3() { Choose(3); }
    UFUNCTION() void OnExitClicked();

    void BindDialogue(UPISDialogueComponent* InComp);

protected:
    UPROPERTY(Transient) TObjectPtr<APISCharacter> CachedPlayer;
    UPROPERTY(Transient) TObjectPtr<UPISDialogueComponent> Dialogue;

    UPROPERTY(Transient) TObjectPtr<UVerticalBox> ChoicesBox;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> SpeakerText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> BodyText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> HintText;
    UPROPERTY(Transient) TObjectPtr<UButton> ExitBtn;
    UPROPERTY(Transient) TObjectPtr<UButton> ChoiceBtns[4];

    void Choose(int32 Index);
};
