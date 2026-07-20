// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISInventoryRowWidget.h"
#include "PISInventoryWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void UPISInventoryRowWidget::Init(UPISInventoryWidget* InOwner, int32 InIndex, const FString& InName, int32 InCount, bool bInEquipped)
{
    OwnerWidget = InOwner;
    Index = InIndex;
    bEquipped = bInEquipped;
    if (NameText)
    {
        NameText->SetText(FText::FromString(FString::Printf(TEXT(" %s  x%d%s"), *InName, InCount, bInEquipped ? TEXT("  [E]") : TEXT(""))));
    }
    SetSelected(false);
}

void UPISInventoryRowWidget::OnClicked()
{
    if (OwnerWidget.IsValid())
    {
        OwnerWidget->SelectIndex(Index);
    }
}

void UPISInventoryRowWidget::SetSelected(bool bSelected)
{
    if (Bg)
    {
        Bg->SetBrushColor(bSelected ? FLinearColor(0.30f, 0.20f, 0.06f, 0.95f)
                                    : FLinearColor(0.10f, 0.08f, 0.06f, 0.95f));
    }
}

TSharedRef<SWidget> UPISInventoryRowWidget::RebuildWidget()
{
    if (!WidgetTree) { return Super::RebuildWidget(); }
    WidgetTree->RootWidget = nullptr;

    Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RowBg"));
    Bg->SetBrushColor(FLinearColor(0.10f, 0.08f, 0.06f, 0.95f));
    WidgetTree->RootWidget = Bg;

    Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("RowBtn"));
    Bg->SetContent(Btn);

    NameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RowName"));
    FSlateFontInfo F = NameText->GetFont();
    F.Size = 13;
    NameText->SetFont(F);
    NameText->SetText(FText::FromString(TEXT(" ...")));
    NameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.96f, 0.93f, 0.84f)));
    Btn->AddChild(NameText);
    Btn->OnClicked.AddDynamic(this, &UPISInventoryRowWidget::OnClicked);

    SetSelected(false);
    return Super::RebuildWidget();
}
