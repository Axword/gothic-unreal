// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISJournalWidget.h"
#include "PISCharacter.h"
#include "PISQuestComponent.h"
#include "PISJsonDataSubsystem.h"
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
#include "Dom/JsonObject.h"

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
    UTextBlock* FLabel(UWidgetTree* T, FName N, const FString& S, int32 Size, FLinearColor C)
    {
        UTextBlock* W = Label(T, N, S, Size);
        W->SetColorAndOpacity(FSlateColor(C));
        return W;
    }
}

void UPISJournalWidget::NativeConstruct()
{
    Super::NativeConstruct();
    Refresh();
}

TSharedRef<SWidget> UPISJournalWidget::RebuildWidget()
{
    if (!WidgetTree) { return Super::RebuildWidget(); }
    WidgetTree->RootWidget = nullptr;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("J_Root"));
    WidgetTree->RootWidget = Root;

    UBorder* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("J_Bg"));
    Bg->SetBrushColor(FLinearColor(0.06f, 0.05f, 0.04f, 0.92f));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Bg));
        CS->SetAnchors(FAnchors(0.5f, 0.5f));
        CS->SetAlignment(FVector2D(0.5f, 0.5f));
        CS->SetSize(FVector2D(720.f, 600.f));
    }

    UVerticalBox* Col = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("J_Col"));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Col));
        CS->SetAnchors(FAnchors(0.5f, 0.5f));
        CS->SetAlignment(FVector2D(0.5f, 0.5f));
        CS->SetSize(FVector2D(700.f, 580.f));
    }
    HeaderText = Label(WidgetTree, TEXT("J_Header"), TEXT("Dziennik (J)"), 22);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(HeaderText);
        VS->SetPadding(FMargin(8, 8, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    ListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("J_List"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(ListBox);
        VS->SetPadding(FMargin(8, 0, 8, 0));
        VS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }
    CloseBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("J_Close"));
    CloseBtn->AddChild(Label(WidgetTree, TEXT("J_CloseL"), TEXT("Zamknij (Esc)"), 12));
    CloseBtn->OnClicked.AddDynamic(this, &UPISJournalWidget::OnCloseClicked);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(CloseBtn);
        VS->SetPadding(FMargin(8, 6, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    HintText = Label(WidgetTree, TEXT("J_Hint"), TEXT("Zielony = aktywne, złoty = ukończone, czerwony = nieudane."), 12);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(HintText);
        VS->SetPadding(FMargin(8, 0, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    return Super::RebuildWidget();
}

void UPISJournalWidget::Refresh()
{
    if (!ListBox) { return; }
    ListBox->ClearChildren();
    APlayerController* PC = GetOwningPlayer();
    CachedPlayer = PC ? Cast<APISCharacter>(PC->GetPawn()) : nullptr;
    if (!CachedPlayer) { return; }
    UPISQuestComponent* Quest = CachedPlayer->FindComponentByClass<UPISQuestComponent>();
    if (!Quest) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    if (!Data) { return; }

    auto AddSection = [&](const FString& Title, EPISQuestStatus Status, FLinearColor Color)
    {
        UTextBlock* H = FLabel(WidgetTree, FName(*FString::Printf(TEXT("J_H_%s"), *Title)), Title, 16, Color);
        UVerticalBoxSlot* VS = ListBox->AddChildToVerticalBox(H);
        VS->SetPadding(FMargin(0, 8, 0, 4));
        bool bAny = false;
        for (const FPISQuestState& S : Quest->States)
        {
            if (S.Status != Status) { continue; }
            bAny = true;
            const TSharedPtr<FJsonObject>* O = Data->Find(S.QuestId);
            FString Name = S.QuestId;
            FString StageText;
            if (O && O->IsValid())
            {
                FString T1; if ((*O)->TryGetStringField(TEXT("name"), T1)) { Name = T1; }
                const TArray<TSharedPtr<FJsonValue>>* Journal = nullptr;
                if ((*O)->TryGetArrayField(TEXT("journal"), Journal) && Journal)
                {
                    for (const TSharedPtr<FJsonValue>& V : *Journal)
                    {
                        const TSharedPtr<FJsonObject> E = V.IsValid() ? V->AsObject() : nullptr;
                        if (!E.IsValid()) { continue; }
                        if (E->GetStringField(TEXT("stage")) == S.StageId)
                        {
                            FString TT; if (E->TryGetStringField(TEXT("text"), TT)) { StageText = TT; }
                            break;
                        }
                    }
                }
            }
            UBorder* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
            Bg->SetBrushColor(FLinearColor(0.10f, 0.08f, 0.06f, 0.95f));
            UVerticalBox* V = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
            Bg->SetContent(V);
            V->AddChildToVerticalBox(FLabel(WidgetTree, FName(*FString::Printf(TEXT("J_Q_%s"), *S.QuestId)),
                FString::Printf(TEXT(" %s"), *Name), 14, Color));
            if (!StageText.IsEmpty())
            {
                V->AddChildToVerticalBox(FLabel(WidgetTree, FName(*FString::Printf(TEXT("J_J_%s"), *S.QuestId)),
                    FString::Printf(TEXT("  %s"), *StageText), 12, FLinearColor(0.85f, 0.84f, 0.80f)));
            }
            UVerticalBoxSlot* VSB = ListBox->AddChildToVerticalBox(Bg);
            VSB->SetPadding(FMargin(0, 2, 0, 2));
            VSB->SetHorizontalAlignment(HAlign_Fill);
        }
        if (!bAny)
        {
            UTextBlock* Empty = Label(WidgetTree, FName(*FString::Printf(TEXT("J_E_%s"), *Title)), TEXT(" (brak)"), 12);
            UVerticalBoxSlot* VSE = ListBox->AddChildToVerticalBox(Empty);
            VSE->SetHorizontalAlignment(HAlign_Left);
        }
    };
    AddSection(TEXT("Aktywne"), EPISQuestStatus::Active,    FLinearColor(0.55f, 0.85f, 0.55f));
    AddSection(TEXT("Ukończone"), EPISQuestStatus::Completed, FLinearColor(0.95f, 0.80f, 0.30f));
    AddSection(TEXT("Nieudane"), EPISQuestStatus::Failed,    FLinearColor(0.85f, 0.35f, 0.30f));
}

void UPISJournalWidget::OnCloseClicked() { Close(); }
void UPISJournalWidget::Close() { RemoveFromParent(); }
