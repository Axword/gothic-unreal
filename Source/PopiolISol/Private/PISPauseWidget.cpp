// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISPauseWidget.h"
#include "PISGameInstance.h"
#include "PISSaveGameSubsystem.h"
#include "PISCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundClass.h"

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

void UPISPauseWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UGameUserSettings* Settings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
    if (Settings && MasterSlider) { MasterSlider->SetValue(Settings->GetOverallVolume()); }
    if (Settings && MusicSlider)  { MusicSlider->SetValue(Settings->GetMusicVolume()); }
    if (Settings && SfxSlider)    { SfxSlider->SetValue(Settings->GetSFXVolume()); }
}

TSharedRef<SWidget> UPISPauseWidget::RebuildWidget()
{
    if (!WidgetTree) { return Super::RebuildWidget(); }
    WidgetTree->RootWidget = nullptr;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("P_Root"));
    WidgetTree->RootWidget = Root;

    UBorder* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("P_Bg"));
    Bg->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.05f, 0.92f));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Bg));
        CS->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
    }

    UVerticalBox* Col = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("P_Col"));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Col));
        CS->SetAnchors(FAnchors(0.5f, 0.5f));
        CS->SetAlignment(FVector2D(0.5f, 0.5f));
        CS->SetSize(FVector2D(540.f, 600.f));
    }
    auto Header = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("P_H"));
    FSlateFontInfo HF = Header->GetFont(); HF.Size = 26; Header->SetFont(HF);
    Header->SetText(FText::FromString(TEXT("Pauza (Esc)")));
    Header->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.78f, 0.30f)));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(Header);
        VS->SetPadding(FMargin(8, 12, 8, 12));
        VS->SetHorizontalAlignment(HAlign_Center);
    }
    // Save section
    UTextBlock* SaveH = Label(WidgetTree, TEXT("P_SaveH"), TEXT("Zapis / Wczytanie"), 16);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(SaveH);
        VS->SetPadding(FMargin(8, 8, 8, 4));
    }
    UHorizontalBox* SlotRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("P_SlotRow"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(SlotRow);
        VS->SetPadding(FMargin(8, 2, 8, 2));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    UTextBlock* SlotL = Label(WidgetTree, TEXT("P_SlotL"), TEXT("Slot:"), 13);
    UHorizontalBoxSlot* SLS = Cast<UHorizontalBoxSlot>(SlotRow->AddChild(SlotL));
    SLS->SetPadding(FMargin(0, 0, 8, 0));
    SLS->SetSize(FSlateChildSize(ESlateSizeRule::Fill, 0.2f));
    SlotInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("P_SlotIn"));
    SlotInput->SetText(FText::FromString(TEXT("default")));
    UHorizontalBoxSlot* SIS = Cast<UHorizontalBoxSlot>(SlotRow->AddChild(SlotInput));
    SIS->SetSize(FSlateChildSize(ESlateSizeRule::Fill, 0.8f));
    SlotInput->OnTextChanged.AddDynamic(this, &UPISPauseWidget::OnSlotTextChanged);

    UButton* SaveBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("P_Save"));
    SaveBtn->AddChild(Label(WidgetTree, TEXT("P_SaveL"), TEXT("Zapisz"), 13));
    SaveBtn->OnClicked.AddDynamic(this, &UPISPauseWidget::OnSaveClicked);
    UButton* LoadBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("P_Load"));
    LoadBtn->AddChild(Label(WidgetTree, TEXT("P_LoadL"), TEXT("Wczytaj"), 13));
    LoadBtn->OnClicked.AddDynamic(this, &UPISPauseWidget::OnLoadClicked);
    UHorizontalBox* SAct = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("P_SAct"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(SAct);
        VS->SetPadding(FMargin(8, 2, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    UHorizontalBoxSlot* SB1 = Cast<UHorizontalBoxSlot>(SAct->AddChild(SaveBtn));
    SB1->SetPadding(FMargin(4, 2)); SB1->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    UHorizontalBoxSlot* SB2 = Cast<UHorizontalBoxSlot>(SAct->AddChild(LoadBtn));
    SB2->SetPadding(FMargin(4, 2)); SB2->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

    StatusText = Label(WidgetTree, TEXT("P_Status"), TEXT(""), 12);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(StatusText);
        VS->SetPadding(FMargin(8, 0, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Center);
    }

    // Options section
    UTextBlock* OptH = Label(WidgetTree, TEXT("P_OptH"), TEXT("Opcje"), 16);
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(OptH);
        VS->SetPadding(FMargin(8, 8, 8, 4));
    }
    auto MakeSlider = [&](const TCHAR* Name, float& OutSlider) -> UWidget*
    {
        UTextBlock* L = Label(WidgetTree, FName(*FString::Printf(TEXT("P_L_%s"), Name)), FString::Printf(TEXT("%s"), Name), 12);
        UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(*FString::Printf(TEXT("P_R_%s"), Name)));
        UHorizontalBoxSlot* Ls = Cast<UHorizontalBoxSlot>(Row->AddChild(L));
        Ls->SetSize(FSlateChildSize(ESlateSizeRule::Fill, 0.3f));
        USlider* S = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), FName(*FString::Printf(TEXT("P_S_%s"), Name)));
        S->SetMinValue(0.f); S->SetMaxValue(1.f); S->SetValue(0.8f);
        UHorizontalBoxSlot* Ss = Cast<UHorizontalBoxSlot>(Row->AddChild(S));
        Ss->SetSize(FSlateChildSize(ESlateSizeRule::Fill, 0.7f));
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(Row);
        VS->SetPadding(FMargin(8, 2, 8, 2));
        VS->SetHorizontalAlignment(HAlign_Fill);
        return S;
    };
    MasterSlider = Cast<USlider>(MakeSlider(TEXT("Master"), 0));
    MusicSlider  = Cast<USlider>(MakeSlider(TEXT("Music"), 0));
    SfxSlider    = Cast<USlider>(MakeSlider(TEXT("SFX"), 0));
    MasterSlider->OnValueChanged.AddDynamic(this, &UPISPauseWidget::OnMasterChanged);
    MusicSlider->OnValueChanged.AddDynamic(this, &UPISPauseWidget::OnMusicChanged);
    SfxSlider->OnValueChanged.AddDynamic(this, &UPISPauseWidget::OnSfxChanged);

    UHorizontalBox* InvertRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("P_InvR"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(InvertRow);
        VS->SetPadding(FMargin(8, 4, 8, 4));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    UTextBlock* InvertL = Label(WidgetTree, TEXT("P_InvL"), TEXT("Odwrócona oś Y myszy"), 12);
    UHorizontalBoxSlot* ILS = Cast<UHorizontalBoxSlot>(InvertRow->AddChild(InvertL));
    ILS->SetSize(FSlateChildSize(ESlateSizeRule::Fill, 0.7f));
    InvertY = WidgetTree->ConstructWidget<UCheckBox>(UCheckBox::StaticClass(), TEXT("P_Inv"));
    UHorizontalBoxSlot* ICS = Cast<UHorizontalBoxSlot>(InvertRow->AddChild(InvertY));
    ICS->SetSize(FSlateChildSize(ESlateSizeRule::Fill, 0.3f));
    InvertY->OnCheckStateChanged.AddDynamic(this, &UPISPauseWidget::OnInvertYChanged);

    // Resume / Quit
    UHorizontalBox* RQ = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("P_RQ"));
    {
        UVerticalBoxSlot* VS = Col->AddChildToVerticalBox(RQ);
        VS->SetPadding(FMargin(8, 8, 8, 8));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    UButton* Res = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("P_Res"));
    Res->AddChild(Label(WidgetTree, TEXT("P_ResL"), TEXT("Wznów"), 13));
    Res->OnClicked.AddDynamic(this, &UPISPauseWidget::OnResumeClicked);
    UButton* Quit = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("P_Quit"));
    Quit->AddChild(Label(WidgetTree, TEXT("P_QuitL"), TEXT("Wyjdź do menu"), 13));
    Quit->OnClicked.AddDynamic(this, &UPISPauseWidget::OnQuitClicked);
    UHorizontalBoxSlot* RS = Cast<UHorizontalBoxSlot>(RQ->AddChild(Res));
    RS->SetPadding(FMargin(4, 2)); RS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    UHorizontalBoxSlot* QS = Cast<UHorizontalBoxSlot>(RQ->AddChild(Quit));
    QS->SetPadding(FMargin(4, 2)); QS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

    return Super::RebuildWidget();
}

void UPISPauseWidget::OnSaveClicked() { Save(); }
void UPISPauseWidget::OnLoadClicked() { QuickLoad(); }
void UPISPauseWidget::OnResumeClicked() { Resume(); }
void UPISPauseWidget::OnQuitClicked() { QuitToMenu(); }

void UPISPauseWidget::Save()
{
    APISGameInstance* GI = Cast<APISGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI || !GI->GetSaves()) { return; }
    const FString Slot = SlotInput ? SlotInput->GetText().ToString() : TEXT("default");
    APISCharacter* P = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!P) { return; }
    const bool bOk = GI->GetSaves()->SaveSlot(Slot, P);
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(bOk ? FString::Printf(TEXT("Zapisano: %s"), *Slot)
                                                 : FString::Printf(TEXT("Błąd: %s"), *GI->GetSaves()->GetLastError())));
    }
}

void UPISPauseWidget::QuickLoad()
{
    APISGameInstance* GI = Cast<APISGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI || !GI->GetSaves()) { return; }
    const FString Slot = SlotInput ? SlotInput->GetText().ToString() : TEXT("default");
    APISCharacter* P = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!P) { return; }
    const bool bOk = GI->GetSaves()->LoadSlot(Slot, P);
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(bOk ? FString::Printf(TEXT("Wczytano: %s"), *Slot)
                                                 : FString::Printf(TEXT("Błąd: %s"), *GI->GetSaves()->GetLastError())));
    }
}

void UPISPauseWidget::Resume()
{
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->SetPause(false);
        PC->bShowMouseCursor = false;
    }
    RemoveFromParent();
}

void UPISPauseWidget::QuitToMenu()
{
    UGameplayStatics::OpenLevel(this, FName(TEXT("MainMenu")));
}

void UPISPauseWidget::OnSlotTextChanged(const FText& T) { /* could persist */ }
void UPISPauseWidget::OnMasterChanged(float V)
{
    if (GEngine) { GEngine->GetGameUserSettings()->SetOverallVolume(V); GEngine->GetGameUserSettings()->ApplyNonResolutionSettings(); }
}
void UPISPauseWidget::OnMusicChanged(float V)
{
    if (GEngine) { GEngine->GetGameUserSettings()->SetMusicVolume(V); GEngine->GetGameUserSettings()->ApplyNonResolutionSettings(); }
}
void UPISPauseWidget::OnSfxChanged(float V)
{
    if (GEngine) { GEngine->GetGameUserSettings()->SetSFXVolume(V); GEngine->GetGameUserSettings()->ApplyNonResolutionSettings(); }
}
void UPISPauseWidget::OnInvertYChanged(bool V)
{
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        FRotator R = PC->GetControlRotation();
        R.Roll = V ? 180.f : 0.f;
        PC->SetControlRotation(R);
    }
}
