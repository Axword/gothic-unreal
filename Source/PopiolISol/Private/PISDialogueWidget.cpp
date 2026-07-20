// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISDialogueWidget.h"
#include "PISDialogueComponent.h"
#include "PISCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

namespace
{
    UTextBlock* Label(UWidgetTree* T, FName N, const FString& S, int32 Size = 14)
    {
        UTextBlock* W = T->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), N);
        FSlateFontInfo F = W->GetFont();
        F.Size = Size;
        W->SetFont(F);
        W->SetText(FText::FromString(S));
        W->SetColorAndOpacity(FSlateColor(FLinearColor(0.96f, 0.93f, 0.84f)));
        return W;
    }
}

void UPISDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
    APlayerController* PC = GetOwningPlayer();
    CachedPlayer = PC ? Cast<APISCharacter>(PC->GetPawn()) : nullptr;
    if (CachedPlayer)
    {
        Dialogue = CachedPlayer->FindComponentByClass<UPISDialogueComponent>();
    }
}

void UPISDialogueWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

void UPISDialogueWidget::BindDialogue(UPISDialogueComponent* InComp)
{
    if (Dialogue && Dialogue != InComp)
    {
        Dialogue->OnNodeShown.RemoveDynamic(this, &UPISDialogueWidget::OnNodeShown);
    }
    Dialogue = InComp;
    if (Dialogue)
    {
        Dialogue->OnNodeShown.AddDynamic(this, &UPISDialogueWidget::OnNodeShown);
    }
}

TSharedRef<SWidget> UPISDialogueWidget::RebuildWidget()
{
    if (!WidgetTree) { return Super::RebuildWidget(); }
    WidgetTree->RootWidget = nullptr;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("D_Root"));
    WidgetTree->RootWidget = Root;

    UBorder* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("D_Bg"));
    Bg->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.05f, 0.85f));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Bg));
        CS->SetAnchors(FAnchors(0.5f, 1.f));
        CS->SetAlignment(FVector2D(0.5f, 1.f));
        CS->SetSize(FVector2D(820.f, 280.f));
        CS->SetPosition(FVector2D(0.f, -20.f));
    }

    UVerticalBox* Col = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("D_Col"));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Col));
        CS->SetAnchors(FAnchors(0.5f, 1.f));
        CS->SetAlignment(FVector2D(0.5f, 1.f));
        CS->SetSize(FVector2D(800.f, 260.f));
        CS->SetPosition(FVector2D(0.f, -20.f));
    }
    SpeakerText = Label(WidgetTree, TEXT("D_Speaker"), TEXT("..."), 16);
    SpeakerText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.78f, 0.30f)));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(SpeakerText);
        VS->SetPadding(FMargin(12, 8, 12, 4));
    }
    BodyText = Label(WidgetTree, TEXT("D_Body"), TEXT(""), 14);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(BodyText);
        VS->SetPadding(FMargin(12, 4, 12, 8));
        VS->SetAutoSize(true);
    }
    ChoicesBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("D_Choices"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(ChoicesBox);
        VS->SetPadding(FMargin(12, 0, 12, 0));
        VS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }
    for (int32 i = 0; i < 4; ++i)
    {
        ChoiceBtns[i] = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(),
            FName(*FString::Printf(TEXT("D_C%d"), i)));
        UTextBlock* T = Label(WidgetTree, FName(*FString::Printf(TEXT("D_CL%d"), i)), TEXT(""), 12);
        ChoiceBtns[i]->AddChild(T);
        UVerticalBoxSlot* VS = ChoicesBox->AddChildToVerticalBox(ChoiceBtns[i]);
        VS->SetPadding(FMargin(0, 2, 0, 2));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    ExitBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("D_Exit"));
    ExitBtn->AddChild(Label(WidgetTree, TEXT("D_ExitL"), TEXT("Zakończ (Esc)"), 12));
    ExitBtn->OnClicked.AddDynamic(this, &UPISDialogueWidget::OnExitClicked);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(ExitBtn);
        VS->SetPadding(FMargin(12, 4, 12, 4));
        VS->SetHorizontalAlignment(HAlign_Right);
    }
    HintText = Label(WidgetTree, TEXT("D_Hint"), TEXT("Kliknij odpowiedź. Wybór zapisywany jest natychmiast."), 11);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(HintText);
        VS->SetPadding(FMargin(12, 0, 12, 8));
    }
    // Wire dynamic delegate for choices
    ChoiceBtns[0]->OnClicked.AddDynamic(this, &UPISDialogueWidget::OnChoice0);
    ChoiceBtns[1]->OnClicked.AddDynamic(this, &UPISDialogueWidget::OnChoice1);
    ChoiceBtns[2]->OnClicked.AddDynamic(this, &UPISDialogueWidget::OnChoice2);
    ChoiceBtns[3]->OnClicked.AddDynamic(this, &UPISDialogueWidget::OnChoice3);
    return Super::RebuildWidget();
}

void UPISDialogueWidget::OnNodeShown(const FPISDialogueNodeView& Node)
{
    if (SpeakerText) { SpeakerText->SetText(FText::FromString(Node.Speaker.IsEmpty() ? TEXT("Nieznajomy") : Node.Speaker)); }
    if (BodyText)    { BodyText->SetText(FText::FromString(Node.Text)); }
    for (int32 i = 0; i < 4; ++i)
    {
        if (!ChoiceBtns[i]) { continue; }
        if (Node.Choices.IsValidIndex(i) && Node.Choices[i].bEnabled)
        {
            ChoiceBtns[i]->SetVisibility(ESlateVisibility::Visible);
            UTextBlock* T = Cast<UTextBlock>(ChoiceBtns[i]->GetChildAt(0));
            if (T) { T->SetText(FText::FromString(FString::Printf(TEXT("%d. %s"), i + 1, *Node.Choices[i].Text))); }
        }
        else
        {
            ChoiceBtns[i]->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UPISDialogueWidget::Choose(int32 Index)
{
    if (!Dialogue) { return; }
    Dialogue->ChooseByIndex(Index);
    if (Dialogue->IsOpen())
    {
        OnNodeShown(Dialogue->GetCurrentNode());
    }
    else
    {
        RemoveFromParent();
    }
}

void UPISDialogueWidget::OnExitClicked()
{
    if (Dialogue) { Dialogue->CloseDialogue(); }
    RemoveFromParent();
}
