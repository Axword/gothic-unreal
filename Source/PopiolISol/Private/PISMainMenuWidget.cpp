// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISMainMenuWidget.h"
#include "PISPauseWidget.h"
#include "PISSaveGameSubsystem.h"
#include "PISGameInstance.h"
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

void UPISMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    APlayerController* PC = GetOwningPlayer();
    if (PC) { PC->bShowMouseCursor = true; }
}

TSharedRef<SWidget> UPISMainMenuWidget::RebuildWidget()
{
    if (!WidgetTree) { return Super::RebuildWidget(); }
    WidgetTree->RootWidget = nullptr;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("MM_Root"));
    WidgetTree->RootWidget = Root;

    UBorder* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("MM_Bg"));
    Bg->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.05f, 0.92f));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Bg));
        CS->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
    }

    UVerticalBox* Col = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MM_Col"));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Col));
        CS->SetAnchors(FAnchors(0.5f, 0.5f));
        CS->SetAlignment(FVector2D(0.5f, 0.5f));
        CS->SetSize(FVector2D(480.f, 480.f));
    }
    UTextBlock* Title = Label(WidgetTree, TEXT("MM_T"), TEXT("Popiół i Sól"), 36);
    Title->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.78f, 0.30f)));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(Title);
        VS->SetPadding(FMargin(8, 16, 8, 24));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    UTextBlock* Sub = Label(WidgetTree, TEXT("MM_S"), TEXT("Kraina, która pamięta wybór"), 14);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(Sub);
        VS->SetPadding(FMargin(8, 0, 8, 24));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    MenuBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MM_Menu"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(MenuBox);
        VS->SetPadding(FMargin(8, 0, 8, 0));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    auto AddBtn = [&](const TCHAR* Name, const TCHAR* LabelStr, void(UPISMainMenuWidget::*Handler)()) -> UButton*
    {
        UButton* B = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(Name));
        B->AddChild(Label(WidgetTree, FName(*FString::Printf(TEXT("%s_L"), Name)), LabelStr, 16));
        B->OnClicked.AddDynamic(this, Handler);
        UVerticalBoxSlot* VS = MenuBox->AddChildToVerticalBox(B);
        VS->SetPadding(FMargin(8, 4, 8, 4));
        VS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        return B;
    };
    NewBtn  = AddBtn(TEXT("MM_New"),  TEXT("Nowa gra"),  &UPISMainMenuWidget::OnNewGame);
    LoadBtn = AddBtn(TEXT("MM_Load"), TEXT("Wczytaj"),   &UPISMainMenuWidget::OnLoadGame);
    OptBtn  = AddBtn(TEXT("MM_Opt"),  TEXT("Opcje"),     &UPISMainMenuWidget::OnOptions);
    QuitBtn = AddBtn(TEXT("MM_Q"),    TEXT("Wyjdź"),     &UPISMainMenuWidget::OnQuit);

    StatusText = Label(WidgetTree, TEXT("MM_Status"), TEXT("Witaj w Słonawym Przesmyku."), 12);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(StatusText);
        VS->SetPadding(FMargin(8, 24, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    return Super::RebuildWidget();
}

void UPISMainMenuWidget::OnNewGame()
{
    UGameplayStatics::OpenLevel(this, FName(TEXT("Prototype")));
}

void UPISMainMenuWidget::OnLoadGame()
{
    APISGameInstance* GI = Cast<APISGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI || !GI->GetSaves()) { return; }
    APISCharacter* P = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!P) { return; }
    const TArray<FPISSaveSlot> Slots = GI->GetSaves()->ListSlots();
    if (Slots.Num() == 0)
    {
        if (StatusText) { StatusText->SetText(FText::FromString(TEXT("Brak zapisów."))); }
        return;
    }
    const bool bOk = GI->GetSaves()->LoadSlot(Slots[0].SlotName, P);
    if (StatusText) { StatusText->SetText(FText::FromString(bOk ? TEXT("Wczytano.") : GI->GetSaves()->GetLastError())); }
    if (bOk) { UGameplayStatics::OpenLevel(this, FName(TEXT("Prototype"))); }
}

void UPISMainMenuWidget::OnOptions()
{
    ShowPauseOverlay();
}

void UPISMainMenuWidget::OnQuit()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->ConsoleCommand(TEXT("quit"), true);
    }
}

void UPISMainMenuWidget::ShowPauseOverlay()
{
    if (!PauseOverlay)
    {
        PauseOverlay = WidgetTree->ConstructWidget<UPISPauseWidget>(UPISPauseWidget::StaticClass(), TEXT("MM_Pause"));
    }
    if (PauseOverlay && !PauseOverlay->IsInViewport())
    {
        PauseOverlay->AddToViewport(100);
    }
}

void UPISMainMenuWidget::OnResumePause()
{
    if (PauseOverlay && PauseOverlay->IsInViewport())
    {
        PauseOverlay->RemoveFromParent();
    }
}
