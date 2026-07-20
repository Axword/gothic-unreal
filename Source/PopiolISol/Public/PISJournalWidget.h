// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PISJournalWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UButton;
class UBorder;
class APISCharacter;

UCLASS()
class POPIOLISOL_API UPISJournalWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;

    UFUNCTION(BlueprintCallable, Category = "Journal")
    void Refresh();

    UFUNCTION(BlueprintCallable, Category = "Journal")
    void Close();

protected:
    UPROPERTY(Transient) TObjectPtr<APISCharacter> CachedPlayer;
    UPROPERTY(Transient) TObjectPtr<UVerticalBox> ListBox;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> HeaderText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> HintText;
    UPROPERTY(Transient) TObjectPtr<UButton> CloseBtn;

    UFUNCTION() void OnCloseClicked();
};
