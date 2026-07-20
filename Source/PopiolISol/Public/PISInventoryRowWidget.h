// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PISInventoryRowWidget.generated.h"

class UTextBlock;
class UButton;
class UPISInventoryWidget;

UCLASS()
class POPIOLISOL_API UPISInventoryRowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Wire this row to its owning inventory widget and index. */
    void Init(UPISInventoryWidget* InOwner, int32 InIndex, const FString& InName, int32 InCount, bool bInEquipped);

    UFUNCTION() void OnClicked();

    void SetSelected(bool bSelected);

    virtual TSharedRef<SWidget> RebuildWidget() override;

protected:
    UPROPERTY(Transient) TWeakObjectPtr<UPISInventoryWidget> OwnerWidget;
    UPROPERTY(Transient) int32 Index = -1;
    UPROPERTY(Transient) bool bEquipped = false;

    UPROPERTY(Transient) TObjectPtr<UTextBlock> NameText;
    UPROPERTY(Transient) TObjectPtr<UButton> Btn;
    UPROPERTY(Transient) TObjectPtr<UBorder> Bg;
};
