// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISHUDWidget.h"
#include "PISCharacter.h"
#include "PISStatsComponent.h"
#include "PISInventoryComponent.h"
#include "PISWorldClock.h"
#include "PISJsonDataSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

namespace
{
    UTextBlock* MakeText(UWidgetTree* Tree, FName Name, const FString& Initial, int32 Size = 14)
    {
        UTextBlock* T = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
        FSlateFontInfo F = T->GetFont();
        F.Size = Size;
        T->SetFont(F);
        T->SetText(FText::FromString(Initial));
        T->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.94f, 0.86f)));
        return T;
    }
}

void UPISHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshFromPlayer();
}

TSharedRef<SWidget> UPISHUDWidget::RebuildWidget()
{
    if (!WidgetTree) { return Super::RebuildWidget(); }
    WidgetTree->RootWidget = nullptr;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("HUD_Root"));
    WidgetTree->RootWidget = Root;

    // ----- Top-left: HP / Mana bars -----
    UVerticalBox* Bars = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Bars"));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Bars));
        CS->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
        CS->SetAutoSize(true);
        CS->SetPosition(FVector2D(20.f, 20.f));
    }

    HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
    HealthBar->SetPercent(1.0f);
    HealthBar->SetFillColorAndOpacity(FLinearColor(0.65f, 0.10f, 0.10f));
    {
        UVerticalBoxSlot* VS = Bars->AddChildToVerticalBox(HealthBar);
        VS->SetPadding(FMargin(0, 0, 0, 4));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    HealthText = MakeText(WidgetTree, TEXT("HealthText"), TEXT("HP 80/80"), 14);
    {
        UVerticalBoxSlot* VS = Bars->AddChildToVerticalBox(HealthText);
        VS->SetPadding(FMargin(0, 0, 0, 8));
        VS->SetHorizontalAlignment(HAlign_Left);
    }

    ManaBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("ManaBar"));
    ManaBar->SetPercent(1.0f);
    ManaBar->SetFillColorAndOpacity(FLinearColor(0.20f, 0.40f, 0.85f));
    {
        UVerticalBoxSlot* VS = Bars->AddChildToVerticalBox(ManaBar);
        VS->SetPadding(FMargin(0, 0, 0, 4));
        VS->SetHorizontalAlignment(HAlign_Fill);
    }
    ManaText = MakeText(WidgetTree, TEXT("ManaText"), TEXT("Mana 50/50"), 14);
    {
        UVerticalBoxSlot* VS = Bars->AddChildToVerticalBox(ManaText);
        VS->SetPadding(FMargin(0, 0, 0, 0));
        VS->SetHorizontalAlignment(HAlign_Left);
    }

    // ----- Top-right: level, gold, weapon -----
    UVerticalBox* Info = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Info"));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(Info));
        CS->SetAnchors(FAnchors(1.f, 0.f, 1.f, 0.f));
        CS->SetAutoSize(true);
        CS->SetPosition(FVector2D(-220.f, 20.f));
        CS->SetAlignment(FVector2D(0.f, 0.f));
    }
    LevelText = MakeText(WidgetTree, TEXT("LevelText"), TEXT("Poziom 1"), 14);
    GoldText  = MakeText(WidgetTree, TEXT("GoldText"),  TEXT("Sól: 0"), 14);
    WeaponText= MakeText(WidgetTree, TEXT("WeaponText"), TEXT("Broń: -"), 14);
    {
        UVerticalBoxSlot* S;
        S = Info->AddChildToVerticalBox(LevelText);  S->SetHorizontalAlignment(HAlign_Right);
        S = Info->AddChildToVerticalBox(GoldText);   S->SetHorizontalAlignment(HAlign_Right);
        S = Info->AddChildToVerticalBox(WeaponText); S->SetHorizontalAlignment(HAlign_Right);
    }

    // ----- Center: message (one-shot) -----
    MessageText = MakeText(WidgetTree, TEXT("MessageText"), TEXT(""), 16);
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(MessageText));
        CS->SetAnchors(FAnchors(0.5f, 0.85f, 0.5f, 0.85f));
        CS->SetAutoSize(true);
        CS->SetAlignment(FVector2D(0.5f, 0.5f));
    }

    // ----- Bottom-left: location, bottom-right: clock -----
    LocationText = MakeText(WidgetTree, TEXT("LocationText"), TEXT("Słonawy Przesmyk"), 13);
    LocationText->SetColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.78f, 0.74f)));
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(LocationText));
        CS->SetAnchors(FAnchors(0.f, 1.f, 0.f, 1.f));
        CS->SetAutoSize(true);
        CS->SetPosition(FVector2D(20.f, -40.f));
        CS->SetAlignment(FVector2D(0.f, 1.f));
    }
    ClockText = MakeText(WidgetTree, TEXT("ClockText"), TEXT("07:00"), 13);
    {
        UCanvasPanelSlot* CS = Cast<UCanvasPanelSlot>(Root->AddChild(ClockText));
        CS->SetAnchors(FAnchors(1.f, 1.f, 1.f, 1.f));
        CS->SetAutoSize(true);
        CS->SetPosition(FVector2D(-20.f, -40.f));
        CS->SetAlignment(FVector2D(1.f, 1.f));
    }
    return Super::RebuildWidget();
}

void UPISHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (MessageRemaining > 0.f)
    {
        MessageRemaining -= InDeltaTime;
        if (MessageRemaining <= 0.f && MessageText)
        {
            MessageText->SetText(FText::GetEmpty());
        }
    }
    RefreshFromPlayer();
}

void UPISHUDWidget::ShowMessage(const FString& Text)
{
    if (MessageText)
    {
        MessageText->SetText(FText::FromString(Text));
        MessageRemaining = 3.f;
    }
}

void UPISHUDWidget::RefreshFromPlayer()
{
    if (!CachedPlayer)
    {
        APlayerController* PC = GetOwningPlayer();
        CachedPlayer = PC ? Cast<APISCharacter>(PC->GetPawn()) : nullptr;
        if (!CachedPlayer) { return; }
    }
    UPISStatsComponent* Stats = CachedPlayer->FindComponentByClass<UPISStatsComponent>();
    UPISInventoryComponent* Inv  = CachedPlayer->FindComponentByClass<UPISInventoryComponent>();
    if (Stats)
    {
        const float MaxHp = FMath::Max(1, Stats->GetMaxHp());
        const float MaxMp = FMath::Max(1, Stats->GetMaxMana());
        if (HealthBar) { HealthBar->SetPercent(Stats->CurrentHp / MaxHp); }
        if (ManaBar)   { ManaBar->SetPercent(Stats->CurrentMana / MaxMp); }
        if (HealthText){ HealthText->SetText(FText::FromString(FString::Printf(TEXT("HP %d/%d"), Stats->CurrentHp, Stats->GetMaxHp()))); }
        if (ManaText)  { ManaText->SetText(FText::FromString(FString::Printf(TEXT("Mana %d/%d"), Stats->CurrentMana, Stats->GetMaxMana()))); }
        if (LevelText) { LevelText->SetText(FText::FromString(FString::Printf(TEXT("Poziom %d  (PN: %d)"), Stats->Level, Stats->LearningPoints))); }
    }
    if (Inv)
    {
        const int32 Gold = Inv->CountItem(TEXT("currency_salt"));
        if (GoldText) { GoldText->SetText(FText::FromString(FString::Printf(TEXT("Sól: %d"), Gold))); }
        FString WeaponName = Inv->EquippedWeaponId;
        if (!WeaponName.IsEmpty())
        {
            UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
            if (UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr)
            {
                const TSharedPtr<FJsonObject>* O = Data->Find(WeaponName);
                if (O && O->IsValid())
                {
                    FString N; if ((*O)->TryGetStringField(TEXT("name"), N)) { WeaponName = N; }
                }
            }
        }
        if (WeaponText) { WeaponText->SetText(FText::FromString(FString::Printf(TEXT("Broń: %s"), *WeaponName))); }
    }
    if (ClockText)
    {
        UWorld* World = GetWorld();
        APISWorldClock* Clock = nullptr;
        for (TActorIterator<APISWorldClock> It(World); It; ++It) { Clock = *It; break; }
        if (Clock)
        {
            const int32 Hour = FMath::FloorToInt(Clock->Hour);
            const int32 Min  = FMath::FloorToInt((Clock->Hour - Hour) * 60.f);
            ClockText->SetText(FText::FromString(FString::Printf(TEXT("Dzień %d  %02d:%02d"), Clock->Day, Hour, Min)));
        }
    }
}
