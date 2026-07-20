// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISNPCController.h"
#include "PISNPC.h"
#include "PISCharacter.h"
#include "PISWorldClock.h"
#include "PISJsonDataSubsystem.h"
#include "PISDialogueComponent.h"
#include "PISStatsComponent.h"
#include "PISInventoryComponent.h"
#include "PISCrimeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "DrawDebugHelpers.h"

APISNPCController::APISNPCController()
{
    PrimaryActorTick.bCanEverTick = true;
    bWantsPlayerState = false;
}

void APISNPCController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    Controlled = Cast<APISNPC>(InPawn);
    if (Controlled)
    {
        Controlled->NpcController = this;
    }
}

namespace
{
    APISWorldClock* FindClock(UWorld* World)
    {
        for (TActorIterator<APISWorldClock> It(World); It; ++It) { return *It; break; }
        return nullptr;
    }
    int32 HourToMinutes(float Hour)
    {
        return FMath::Clamp(FMath::FloorToInt(Hour * 60.f), 0, 24 * 60);
    }
    int32 TimeStrToMinutes(const FString& T)
    {
        // "HH:MM"
        int32 H = 0, M = 0;
        T.Split(TEXT(":"), &H, &M, nullptr, nullptr, ESearchCase::CaseSensitive);
        return FMath::Clamp(H * 60 + M, 0, 24 * 60 - 1);
    }
}

void APISNPCController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!Controlled) { return; }
    UWorld* World = GetWorld();
    if (!World) { return; }
    APISWorldClock* Clock = FindClock(World);
    if (!Clock) { return; }
    if (Clock->Day != LastScheduleDay || CurrentActivity.IsEmpty())
    {
        RefreshSchedule();
    }
}

void APISNPCController::RefreshSchedule()
{
    UWorld* World = GetWorld();
    if (!World || !Controlled) { return; }
    APISWorldClock* Clock = FindClock(World);
    if (!Clock) { return; }
    LastScheduleDay = Clock->Day;
    UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    if (!Data) { return; }
    const TSharedPtr<FJsonObject>* Sch = Data->Find(Controlled->ScheduleId);
    if (!Sch || !Sch->IsValid()) { return; }
    const TArray<TSharedPtr<FJsonValue>>* Slots = nullptr;
    if (!(*Sch)->TryGetArrayField(TEXT("slots"), Slots) || !Slots) { return; }
    const int32 NowMin = HourToMinutes(Clock->Hour);
    const TSharedPtr<FJsonObject>* Active = nullptr;
    for (const TSharedPtr<FJsonValue>& V : *Slots)
    {
        const TSharedPtr<FJsonObject> S = V.IsValid() ? V->AsObject() : nullptr;
        if (!S.IsValid()) { continue; }
        const int32 From = TimeStrToMinutes(S->GetStringField(TEXT("from")));
        const int32 To   = TimeStrToMinutes(S->GetStringField(TEXT("to")));
        bool bInSlot = false;
        if (From < To) { bInSlot = (NowMin >= From && NowMin < To); }
        else           { bInSlot = (NowMin >= From || NowMin < To); } // wraps midnight
        if (bInSlot) { Active = &S; break; }
    }
    if (!Active || !Active->IsValid()) { return; }
    const FString Activity = (*Active)->GetStringField(TEXT("activity"));
    const FString Marker   = (*Active)->GetStringField(TEXT("marker"));
    if (Activity == CurrentActivity && Marker == CurrentMarker) { return; }
    CurrentActivity = Activity;
    CurrentMarker = Marker;
    Controlled->MoveToMarker(Marker);
}

void APISNPCController::OnCrimeWitnessed(FVector At, FString WitnessId)
{
    if (!Controlled) { return; }
    APISCharacter* Player = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Player) { return; }
    UPISStatsComponent* Stats = Player->FindComponentByClass<UPISStatsComponent>();
    if (Stats && Controlled->Faction == TEXT("stary_porzadek"))
    {
        Stats->ModifyReputation(EPISFaction::StaryPorzadek, -3);
    }
    if (Stats && Controlled->Faction == TEXT("nowy_brzeg"))
    {
        Stats->ModifyReputation(EPISFaction::NowyBrzeg, -2);
    }
    // Open dialogue if close enough.
    const float D = FVector::Dist(Controlled->GetActorLocation(), Player->GetActorLocation());
    if (D < 250.f)
    {
        if (UPISDialogueComponent* Dlg = Player->FindComponentByClass<UPISDialogueComponent>())
        {
            FString DlgId = Controlled->DialogueId;
            if (DlgId.IsEmpty()) { DlgId = FString::Printf(TEXT("dialogue_%s_intro"), *Controlled->NpcId); }
            Dlg->OpenDialogue(DlgId);
        }
    }
}
