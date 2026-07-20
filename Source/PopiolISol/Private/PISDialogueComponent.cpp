// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISDialogueComponent.h"
#include "PISJsonDataSubsystem.h"
#include "PISStatsComponent.h"
#include "PISInventoryComponent.h"
#include "PISQuestComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

namespace
{
    FString GetStringField(const TSharedPtr<FJsonObject>& Obj, const FString& Name, const FString& Fallback = FString())
    {
        if (!Obj.IsValid()) { return Fallback; }
        FString Out;
        if (Obj->TryGetStringField(Name, Out)) { return Out; }
        return Fallback;
    }

    bool GetBoolField(const TSharedPtr<FJsonObject>& Obj, const FString& Name, bool Fallback = false)
    {
        if (!Obj.IsValid()) { return Fallback; }
        bool Out = Fallback;
        if (Obj->TryGetBoolField(Name, Out)) { return Out; }
        return Fallback;
    }
}

void UPISDialogueComponent::BuildNodeView(const FString& NodeId, const TSharedPtr<FJsonObject>& NodeObj)
{
    if (!NodeObj.IsValid())
    {
        FinishDialogue();
        return;
    }
    CurrentNodeId = NodeId;
    CurrentNode = FPISDialogueNodeView();
    CurrentNode.Id = NodeId;
    CurrentNode.Speaker = GetStringField(NodeObj, TEXT("speaker"), FString());
    CurrentNode.Text = GetStringField(NodeObj, TEXT("text"), FString());

    const TArray<TSharedPtr<FJsonValue>>* ChoicesArr = nullptr;
    if (NodeObj->TryGetArrayField(TEXT("choices"), ChoicesArr) && ChoicesArr)
    {
        for (const TSharedPtr<FJsonValue>& V : *ChoicesArr)
        {
            const TSharedPtr<FJsonObject> C = V.IsValid() ? V->AsObject() : nullptr;
            if (!C.IsValid()) { continue; }
            FPISDialogueChoice Ch;
            Ch.Id = GetStringField(C, TEXT("id"), FString());
            Ch.Text = GetStringField(C, TEXT("text"), FString());
            Ch.Next = GetStringField(C, TEXT("next"), FString());
            Ch.bIsExit = GetBoolField(C, TEXT("exit"), false);
            Ch.bEnabled = true;
            // Per-choice conditions array.
            const TArray<TSharedPtr<FJsonValue>>* Conds = nullptr;
            if (C->TryGetArrayField(TEXT("conditions"), Conds) && Conds)
            {
                for (const TSharedPtr<FJsonValue>& CV : *Conds)
                {
                    if (!EvaluateCondition(CV.IsValid() ? CV->AsObject() : nullptr))
                    {
                        Ch.bEnabled = false;
                        break;
                    }
                }
            }
            // Implicit exit if no next and actions include exit.
            const TArray<TSharedPtr<FJsonValue>>* Acts = nullptr;
            if (C->TryGetArrayField(TEXT("actions"), Acts) && Acts)
            {
                for (const TSharedPtr<FJsonValue>& AV : *Acts)
                {
                    const TSharedPtr<FJsonObject> AO = AV.IsValid() ? AV->AsObject() : nullptr;
                    if (AO.IsValid() && GetStringField(AO, TEXT("type")) == TEXT("exit"))
                    {
                        Ch.bIsExit = true;
                        break;
                    }
                }
            }
            CurrentNode.Choices.Add(Ch);
        }
    }
    OnNodeShown.Broadcast(CurrentNode);
}

bool UPISDialogueComponent::OpenDialogue(const FString& DialogueId)
{
    UWorld* World = GetWorld();
    if (!World) { return false; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
    if (!GI) { return false; }
    UPISJsonDataSubsystem* Data = GI->GetSubsystem<UPISJsonDataSubsystem>();
    if (!Data) { return false; }

    const TSharedPtr<FJsonObject>* Root = Data->Find(DialogueId);
    if (!Root || !Root->IsValid()) { return false; }
    CurrentDialogueId = DialogueId;

    // Cache the player for condition evaluation.
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    CachedPlayer = PC ? PC->GetPawn() : nullptr;

    const FString StartNode = GetStringField(*Root, TEXT("start"), TEXT("start"));
    const TSharedPtr<FJsonObject>* NodesObj = nullptr;
    if ((*Root)->TryGetObjectField(TEXT("nodes"), NodesObj) && NodesObj && NodesObj->IsValid())
    {
        const TSharedPtr<FJsonObject>* Start = nullptr;
        if ((*NodesObj)->TryGetObjectField(StartNode, Start) && Start && Start->IsValid())
        {
            BuildNodeView(StartNode, *Start);
        }
        else
        {
            // Start node missing - close immediately rather than soft-lock.
            FinishDialogue();
            return false;
        }
    }
    else
    {
        FinishDialogue();
        return false;
    }

    OnOpened.Broadcast(DialogueId);
    return true;
}

void UPISDialogueComponent::CloseDialogue()
{
    if (CurrentDialogueId.IsEmpty()) { return; }
    FinishDialogue();
}

bool UPISDialogueComponent::Choose(const FString& ChoiceId)
{
    if (CurrentDialogueId.IsEmpty()) { return false; }
    UWorld* World = GetWorld();
    if (!World) { return false; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
    if (!GI) { return false; }
    UPISJsonDataSubsystem* Data = GI->GetSubsystem<UPISJsonDataSubsystem>();
    if (!Data) { return false; }

    const TSharedPtr<FJsonObject>* Root = Data->Find(CurrentDialogueId);
    if (!Root || !Root->IsValid()) { return false; }
    const TSharedPtr<FJsonObject>* NodesObj = nullptr;
    if (!(*Root)->TryGetObjectField(TEXT("nodes"), NodesObj) || !NodesObj || !NodesObj->IsValid())
    {
        FinishDialogue();
        return false;
    }
    const TSharedPtr<FJsonObject>* NodeObj = nullptr;
    if (!(*NodesObj)->TryGetObjectField(CurrentNodeId, NodeObj) || !NodeObj || !NodeObj->IsValid())
    {
        FinishDialogue();
        return false;
    }
    const TArray<TSharedPtr<FJsonValue>>* ChoicesArr = nullptr;
    if (!(*NodeObj)->TryGetArrayField(TEXT("choices"), ChoicesArr) || !ChoicesArr) { return false; }

    for (const TSharedPtr<FJsonValue>& V : *ChoicesArr)
    {
        const TSharedPtr<FJsonObject> C = V.IsValid() ? V->AsObject() : nullptr;
        if (!C.IsValid()) { continue; }
        if (GetStringField(C, TEXT("id")) != ChoiceId) { continue; }

        // Run actions in order.
        const TArray<TSharedPtr<FJsonValue>>* Acts = nullptr;
        if (C->TryGetArrayField(TEXT("actions"), Acts) && Acts)
        {
            for (const TSharedPtr<FJsonValue>& AV : *Acts)
            {
                RunAction(AV.IsValid() ? AV->AsObject() : nullptr);
            }
        }

        if (GetBoolField(C, TEXT("exit"), false))
        {
            FinishDialogue();
            return true;
        }
        // Check actions for implicit exit.
        bool bWillExit = false;
        if (Acts)
        {
            for (const TSharedPtr<FJsonValue>& AV : *Acts)
            {
                const TSharedPtr<FJsonObject> AO = AV.IsValid() ? AV->AsObject() : nullptr;
                if (AO.IsValid() && GetStringField(AO, TEXT("type")) == TEXT("exit"))
                {
                    bWillExit = true;
                    break;
                }
            }
        }
        if (bWillExit)
        {
            FinishDialogue();
            return true;
        }

        const FString Next = GetStringField(C, TEXT("next"), FString());
        if (Next.IsEmpty())
        {
            // End of branch.
            FinishDialogue();
            return true;
        }
        const TSharedPtr<FJsonObject>* NextObj = nullptr;
        if ((*NodesObj)->TryGetObjectField(Next, NextObj) && NextObj && NextObj->IsValid())
        {
            BuildNodeView(Next, *NextObj);
        }
        else
        {
            FinishDialogue();
        }
        return true;
    }
    return false;
}

void UPISDialogueComponent::ShowNodeById(const FString& NodeId)
{
    UWorld* World = GetWorld();
    if (!World) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
    if (!GI) { return; }
    UPISJsonDataSubsystem* Data = GI->GetSubsystem<UPISJsonDataSubsystem>();
    if (!Data) { return; }
    const TSharedPtr<FJsonObject>* Root = Data->Find(CurrentDialogueId);
    if (!Root || !Root->IsValid()) { return; }
    const TSharedPtr<FJsonObject>* NodesObj = nullptr;
    if (!(*Root)->TryGetObjectField(TEXT("nodes"), NodesObj) || !NodesObj || !NodesObj->IsValid()) { return; }
    const TSharedPtr<FJsonObject>* NodeObj = nullptr;
    if ((*NodesObj)->TryGetObjectField(NodeId, NodeObj) && NodeObj && NodeObj->IsValid())
    {
        BuildNodeView(NodeId, *NodeObj);
    }
    else
    {
        FinishDialogue();
    }
}

void UPISDialogueComponent::FinishDialogue()
{
    const FString Id = CurrentDialogueId;
    CurrentDialogueId.Empty();
    CurrentNodeId.Empty();
    CurrentNode = FPISDialogueNodeView();
    OnFinished.Broadcast(Id);
}

bool UPISDialogueComponent::EvaluateCondition(const TSharedPtr<FJsonObject>& CondObj) const
{
    if (!CondObj.IsValid()) { return true; }
    const FString Type = GetStringField(CondObj, TEXT("type"), FString()).ToLower();

    UWorld* World = GetWorld();
    UGameInstance* GI = World ? UGameplayStatics::GetGameInstance(World) : nullptr;
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;

    if (Type == TEXT("flag"))
    {
        const FString Flag = GetStringField(CondObj, TEXT("flag"), FString());
        return Flags.Contains(Flag);
    }
    if (Type == TEXT("quest"))
    {
        const FString Qid = GetStringField(CondObj, TEXT("quest_id"), FString());
        const FString Need = GetStringField(CondObj, TEXT("status"), TEXT("completed")).ToLower();
        AActor* Owner = GetOwner();
        UPISQuestComponent* QC = Owner ? Owner->FindComponentByClass<UPISQuestComponent>() : nullptr;
        if (!QC && CachedPlayer.IsValid()) { QC = CachedPlayer->FindComponentByClass<UPISQuestComponent>(); }
        if (!QC) { return false; }
        const EPISQuestStatus S = QC->GetStatus(Qid);
        if (Need == TEXT("completed")) { return S == EPISQuestStatus::Completed; }
        if (Need == TEXT("active"))    { return S == EPISQuestStatus::Active; }
        if (Need == TEXT("failed"))    { return S == EPISQuestStatus::Failed; }
        return S != EPISQuestStatus::Inactive;
    }
    if (Type == TEXT("item"))
    {
        const FString Iid = GetStringField(CondObj, TEXT("item_id"), FString());
        const int32 Min = 1; // quantity could be a future field
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISInventoryComponent* Inv = Owner ? Owner->FindComponentByClass<UPISInventoryComponent>() : nullptr;
        if (!Inv) { return false; }
        return Inv->CountItem(Iid) >= Min;
    }
    if (Type == TEXT("reputation"))
    {
        const FString Fac = GetStringField(CondObj, TEXT("faction"), FString());
        const int32 Min = 0;
        int32 Value = 0;
        if (Fac == TEXT("stary_porzadek")) { Value = 0; } // filled via stats below
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISStatsComponent* Stats = Owner ? Owner->FindComponentByClass<UPISStatsComponent>() : nullptr;
        if (!Stats) { return false; }
        if (Fac == TEXT("stary_porzadek")) { return Stats->GetReputation(EPISFaction::StaryPorzadek) >= Min; }
        if (Fac == TEXT("nowy_brzeg"))     { return Stats->GetReputation(EPISFaction::NowyBrzeg) >= Min; }
        return false;
    }
    if (Type == TEXT("skill"))
    {
        const FString Skill = GetStringField(CondObj, TEXT("skill"), FString());
        const int32 MinRank = 1;
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISStatsComponent* Stats = Owner ? Owner->FindComponentByClass<UPISStatsComponent>() : nullptr;
        if (!Stats) { return false; }
        const FString S = Skill.ToLower();
        if (S == TEXT("sword"))    { return Stats->Skills.Sword >= MinRank; }
        if (S == TEXT("bow"))      { return Stats->Skills.Bow >= MinRank; }
        if (S == TEXT("lockpick")) { return Stats->Skills.Lockpick >= MinRank; }
        if (S == TEXT("steal"))    { return Stats->Skills.Steal >= MinRank; }
        if (S == TEXT("skinning")) { return Stats->Skills.Skinning >= MinRank; }
        if (S == TEXT("fire"))     { return Stats->Skills.FireMagic >= MinRank; }
        if (S == TEXT("ice"))      { return Stats->Skills.IceMagic >= MinRank; }
        return false;
    }
    return true;
}

void UPISDialogueComponent::RunAction(const TSharedPtr<FJsonObject>& ActionObj)
{
    if (!ActionObj.IsValid()) { return; }
    const FString Type = GetStringField(ActionObj, TEXT("type")).ToLower();

    UWorld* World = GetWorld();
    UGameInstance* GI = World ? UGameplayStatics::GetGameInstance(World) : nullptr;

    if (Type == TEXT("set_flag"))
    {
        const FString Flag = GetStringField(ActionObj, TEXT("flag"), FString());
        if (!Flag.IsEmpty()) { Flags.Add(Flag); }
    }
    else if (Type == TEXT("clear_flag"))
    {
        const FString Flag = GetStringField(ActionObj, TEXT("flag"), FString());
        Flags.Remove(Flag);
    }
    else if (Type == TEXT("start_quest"))
    {
        const FString Qid = GetStringField(ActionObj, TEXT("quest_id"), FString());
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISQuestComponent* QC = Owner ? Owner->FindComponentByClass<UPISQuestComponent>() : nullptr;
        if (QC) { QC->StartQuest(Qid, GetStringField(ActionObj, TEXT("stage"), TEXT("start"))); }
    }
    else if (Type == TEXT("set_stage"))
    {
        const FString Qid = GetStringField(ActionObj, TEXT("quest_id"), FString());
        const FString Stage = GetStringField(ActionObj, TEXT("stage"), FString());
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISQuestComponent* QC = Owner ? Owner->FindComponentByClass<UPISQuestComponent>() : nullptr;
        if (QC) { QC->SetStage(Qid, Stage); }
    }
    else if (Type == TEXT("finish_quest"))
    {
        const FString Qid = GetStringField(ActionObj, TEXT("quest_id"), FString());
        const bool bSuccess = GetBoolField(ActionObj, TEXT("success"), true);
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISQuestComponent* QC = Owner ? Owner->FindComponentByClass<UPISQuestComponent>() : nullptr;
        if (QC) { QC->FinishQuest(Qid, bSuccess); }
    }
    else if (Type == TEXT("give_item"))
    {
        const FString Iid = GetStringField(ActionObj, TEXT("item_id"), FString());
        const int32 Count = 1;
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISInventoryComponent* Inv = Owner ? Owner->FindComponentByClass<UPISInventoryComponent>() : nullptr;
        if (Inv) { Inv->AddItem(Iid, Count); }
    }
    else if (Type == TEXT("take_item"))
    {
        const FString Iid = GetStringField(ActionObj, TEXT("item_id"), FString());
        const int32 Count = 1;
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISInventoryComponent* Inv = Owner ? Owner->FindComponentByClass<UPISInventoryComponent>() : nullptr;
        if (Inv) { Inv->RemoveItem(Iid, Count); }
    }
    else if (Type == TEXT("faction_choice"))
    {
        const FString Fac = GetStringField(ActionObj, TEXT("faction"), FString());
        if (!Fac.IsEmpty())
        {
            Flags.Add(FString::Printf(TEXT("faction_choice:%s"), *Fac));
        }
    }
    else if (Type == TEXT("rep"))
    {
        const FString Fac = GetStringField(ActionObj, TEXT("faction"), FString());
        const int32 Delta = 0;
        // For simplicity use 1 if not specified; JSON actions can include "delta".
        int32 D = Delta;
        if (!ActionObj->TryGetNumberField(TEXT("delta"), D))
        {
            D = 1;
        }
        AActor* Owner = CachedPlayer.IsValid() ? CachedPlayer.Get() : GetOwner();
        UPISStatsComponent* Stats = Owner ? Owner->FindComponentByClass<UPISStatsComponent>() : nullptr;
        if (Stats)
        {
            if (Fac == TEXT("stary_porzadek")) { Stats->ModifyReputation(EPISFaction::StaryPorzadek, D); }
            else if (Fac == TEXT("nowy_brzeg")) { Stats->ModifyReputation(EPISFaction::NowyBrzeg, D); }
        }
    }
    else if (Type == TEXT("exit"))
    {
        // Caller handles finishing; nothing to do here.
    }
    // Unrecognised actions are logged once at runtime by the loader; do not soft-lock.
}
