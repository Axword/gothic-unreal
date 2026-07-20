// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISInventoryWidget.h"
#include "PISInventoryRowWidget.h"
#include "PISCharacter.h"
#include "PISInventoryComponent.h"
#include "PISStatsComponent.h"
#include "PISJsonDataSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
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

void UPISInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    Refresh();
}

TSharedRef<SWidget> UPISInventoryWidget::RebuildWidget()
{
    if (!WidgetTree) { return Super::RebuildWidget(); }
    WidgetTree->RootWidget = nullptr;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Inv_Root"));
    WidgetTree->RootWidget = Root;

    UBorder* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Inv_Bg"));
    Bg->SetBrushColor(FLinearColor(0.06f, 0.05f, 0.04f, 0.92f));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Bg));
        CS->SetAnchors(FAnchors(0.5f, 0.5f));
        CS->SetAlignment(FVector2D(0.5f, 0.5f));
        CS->SetSize(FVector2D(640.f, 520.f));
    }

    UVerticalBox* Col = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Inv_Col"));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Col));
        CS->SetAnchors(FAnchors(0.5f, 0.5f));
        CS->SetAlignment(FVector2D(0.5f, 0.5f));
        CS->SetSize(FVector2D(620.f, 500.f));
    }
    HeaderText = Label(WidgetTree, TEXT("Inv_Header"), TEXT("Ekwipunek (I)"), 22);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(HeaderText);
        VS->SetPadding(FMargin(8, 8, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    ListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Inv_List"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(ListBox);
        VS->SetPadding(FMargin(8, 0, 8, 0));
        VS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }
    SelectedText = Label(WidgetTree, TEXT("Inv_Selected"), TEXT("Wybrany: -"), 13);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(SelectedText);
        VS->SetPadding(FMargin(8, 6, 8, 6));
    }
    // Action row.
    UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Inv_Actions"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(Actions);
        VS->SetPadding(FMargin(8, 4, 8, 4));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    UseBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Inv_Use"));
    UseBtn->AddChild(Label(WidgetTree, TEXT("Inv_UseL"), TEXT("Użyj wybrany"), 12));
    UseBtn->OnClicked.AddDynamic(this, &UPISInventoryWidget::OnUseClicked);
    EquipBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Inv_Equip"));
    EquipBtn->AddChild(Label(WidgetTree, TEXT("Inv_EqL"), TEXT("Ekwipuj wybrany"), 12));
    EquipBtn->OnClicked.AddDynamic(this, &UPISInventoryWidget::OnEquipClicked);
    CloseBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Inv_Close"));
    CloseBtn->AddChild(Label(WidgetTree, TEXT("Inv_CloseL"), TEXT("Zamknij (Esc)"), 12));
    CloseBtn->OnClicked.AddDynamic(this, &UPISInventoryWidget::OnCloseClicked);
    {
        UHorizontalBoxSlot* S1 = Cast<UHorizontalBoxSlot>(Actions->AddChild(UseBtn));
        S1->SetPadding(FMargin(4, 2)); S1->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        UHorizontalBoxSlot* S2 = Cast<UHorizontalBoxSlot>(Actions->AddChild(EquipBtn));
        S2->SetPadding(FMargin(4, 2)); S2->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        UHorizontalBoxSlot* S3 = Cast<UHorizontalBoxSlot>(Actions->AddChild(CloseBtn));
        S3->SetPadding(FMargin(4, 2)); S3->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }
    HintText = Label(WidgetTree, TEXT("Inv_Hint"), TEXT("Kliknij wiersz by wybrać, potem Użyj / Ekwipuj / Zamknij."), 12);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(HintText);
        VS->SetPadding(FMargin(8, 6, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    return Super::RebuildWidget();
}

void UPISInventoryWidget::Refresh()
{
    if (!ListBox) { return; }
    ListBox->ClearChildren();
    Rows.Reset();
    CurrentIds.Reset();
    CurrentCounts.Reset();
    APlayerController* PC = GetOwningPlayer();
    CachedPlayer = PC ? Cast<APISCharacter>(PC->GetPawn()) : nullptr;
    if (!CachedPlayer) { return; }
    UPISInventoryComponent* Inv = CachedPlayer->FindComponentByClass<UPISInventoryComponent>();
    if (!Inv) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;

    int32 Index = 0;
    for (const FPISInventoryStack& S : Inv->Items)
    {
        FString Name = S.ItemId;
        if (Data)
        {
            const TSharedPtr<FJsonObject>* O = Data->Find(S.ItemId);
            if (O && O->IsValid())
            {
                FString T1; if ((*O)->TryGetStringField(TEXT("name"), T1)) { Name = T1; }
            }
        }
        UPISInventoryRowWidget* Row = WidgetTree->ConstructWidget<UPISInventoryRowWidget>(UPISInventoryRowWidget::StaticClass(),
            FName(*FString::Printf(TEXT("Row_%d"), Index)));
        const bool bEq = (Inv->EquippedWeaponId == S.ItemId);
        Row->Init(this, Index, Name, S.Count, bEq);
        UVerticalBoxSlot* VS = ListBox->AddChildToVerticalBox(Row);
        VS->SetPadding(FMargin(0, 2, 0, 2));
        VS->SetHorizontalAlignment(HAlign_Fill);
        Rows.Add(Row);
        CurrentIds.Add(S.ItemId);
        CurrentCounts.Add(S.Count);
        ++Index;
    }
    if (Index == 0)
    {
        UTextBlock* Empty = Label(WidgetTree, TEXT("Inv_Empty"), TEXT("(puste)"), 14);
        UVerticalBoxSlot* VS = ListBox->AddChildToVerticalBox(Empty);
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    SelectedIndex = (Index > 0) ? 0 : -1;
    SelectIndex(SelectedIndex);
}

void UPISInventoryWidget::SelectIndex(int32 Index)
{
    SelectedIndex = Index;
    for (int32 i = 0; i < Rows.Num(); ++i)
    {
        if (Rows[i]) { Rows[i]->SetSelected(i == SelectedIndex); }
    }
    if (SelectedText)
    {
        if (CurrentIds.IsValidIndex(SelectedIndex))
        {
            SelectedText->SetText(FText::FromString(FString::Printf(TEXT("Wybrany: %s x%d"), *CurrentIds[SelectedIndex], CurrentCounts[SelectedIndex])));
        }
        else
        {
            SelectedText->SetText(FText::FromString(TEXT("Wybrany: -")));
        }
    }
}

void UPISInventoryWidget::OnUseClicked()  { UseSelected(); }
void UPISInventoryWidget::OnEquipClicked(){ EquipSelected(); }
void UPISInventoryWidget::OnCloseClicked(){ Close(); }

void UPISInventoryWidget::UseSelected()
{
    if (!CurrentIds.IsValidIndex(SelectedIndex)) { return; }
    APlayerController* PC = GetOwningPlayer();
    CachedPlayer = PC ? Cast<APISCharacter>(PC->GetPawn()) : nullptr;
    if (!CachedPlayer) { return; }
    UPISInventoryComponent* Inv = CachedPlayer->FindComponentByClass<UPISInventoryComponent>();
    UPISStatsComponent* Stats = CachedPlayer->FindComponentByClass<UPISStatsComponent>();
    if (!Inv) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    const FString Iid = CurrentIds[SelectedIndex];
    if (Data)
    {
        const TSharedPtr<FJsonObject>* O = Data->Find(Iid);
        if (O && O->IsValid())
        {
            const TSharedPtr<FJsonObject>* Effect = nullptr;
            if ((*O)->TryGetObjectField(TEXT("effect"), Effect) && Effect && Effect->IsValid())
            {
                int32 Heal = 0; if ((*Effect)->TryGetNumberField(TEXT("heal"), Heal) && Stats) { Stats->RestoreHp(Heal); }
                int32 Mana = 0; if ((*Effect)->TryGetNumberField(TEXT("mana"), Mana) && Stats) { Stats->RestoreMana(Mana); }
            }
        }
    }
    Inv->RemoveItem(Iid, 1);
    Refresh();
}

void UPISInventoryWidget::EquipSelected()
{
    if (!CurrentIds.IsValidIndex(SelectedIndex)) { return; }
    APlayerController* PC = GetOwningPlayer();
    CachedPlayer = PC ? Cast<APISCharacter>(PC->GetPawn()) : nullptr;
    if (!CachedPlayer) { return; }
    UPISInventoryComponent* Inv = CachedPlayer->FindComponentByClass<UPISInventoryComponent>();
    if (Inv) { Inv->EquipWeapon(CurrentIds[SelectedIndex]); }
    Refresh();
}

void UPISInventoryWidget::Close()
{
    RemoveFromParent();
}
