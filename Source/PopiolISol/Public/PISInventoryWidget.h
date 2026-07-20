// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PISInventoryWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UButton;
class UBorder;
class UPISInventoryRowWidget;
class APISCharacter;

UCLASS()
class POPIOLISOL_API UPISInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void Refresh();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SelectIndex(int32 Index);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseSelected();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void EquipSelected();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void Close();

    int32 GetSelectedIndex() const { return SelectedIndex; }

protected:
    UPROPERTY(Transient) TObjectPtr<APISCharacter> CachedPlayer;
    UPROPERTY(Transient) TObjectPtr<UVerticalBox> ListBox;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> HeaderText;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> HintText;
    UPROPERTY(Transient) TObjectPtr<UButton> UseBtn;
    UPROPERTY(Transient) TObjectPtr<UButton> EquipBtn;
    UPROPERTY(Transient) TObjectPtr<UButton> CloseBtn;
    UPROPERTY(Transient) TObjectPtr<UTextBlock> SelectedText;

    UFUNCTION() void OnUseClicked();
    UFUNCTION() void OnEquipClicked();
    UFUNCTION() void OnCloseClicked();

    int32 SelectedIndex = 0;
    TArray<FString> CurrentIds;
    TArray<int32>   CurrentCounts;
    TArray<TObjectPtr<UPISInventoryRowWidget>> Rows;
};
