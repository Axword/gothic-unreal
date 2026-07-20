// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISQuestComponent.h"

namespace
{
    FPISQuestState* FindQuestState(TArray<FPISQuestState>& States, const FString& QuestId)
    {
        for (FPISQuestState& State : States)
        {
            if (State.QuestId == QuestId)
            {
                return &State;
            }
        }
        return nullptr;
    }

    const FPISQuestState* FindQuestState(const TArray<FPISQuestState>& States, const FString& QuestId)
    {
        for (const FPISQuestState& State : States)
        {
            if (State.QuestId == QuestId)
            {
                return &State;
            }
        }
        return nullptr;
    }
}

bool UPISQuestComponent::StartQuest(const FString& QuestId, const FString& StartStage)
{
    if (QuestId.IsEmpty())
    {
        return false;
    }
    // Already running / completed.
    if (FindQuestState(States, QuestId) != nullptr)
    {
        return false;
    }
    // Block quest from opposite faction once a faction has been chosen.
    if (IsFactionBlocked(QuestId))
    {
        return false;
    }

    FPISQuestState State;
    State.QuestId = QuestId;
    State.StageId = StartStage;
    State.Status = EPISQuestStatus::Active;
    States.Add(State);
    OnQuestChanged.Broadcast(State);
    return true;
}

bool UPISQuestComponent::SetStage(const FString& QuestId, const FString& StageId)
{
    FPISQuestState* State = FindQuestState(States, QuestId);
    if (State == nullptr || State->Status != EPISQuestStatus::Active || StageId.IsEmpty())
    {
        return false;
    }

    State->StageId = StageId;
    OnQuestChanged.Broadcast(*State);
    return true;
}

bool UPISQuestComponent::FinishQuest(const FString& QuestId, bool bSucceeded)
{
    FPISQuestState* State = FindQuestState(States, QuestId);
    if (State == nullptr || State->Status != EPISQuestStatus::Active)
    {
        return false;
    }

    State->StageId = bSucceeded ? TEXT("done") : TEXT("failed");
    State->Status = bSucceeded ? EPISQuestStatus::Completed : EPISQuestStatus::Failed;
    OnQuestChanged.Broadcast(*State);
    return true;
}

EPISQuestStatus UPISQuestComponent::GetStatus(const FString& QuestId) const
{
    if (const FPISQuestState* State = FindQuestState(States, QuestId))
    {
        return State->Status;
    }
    return EPISQuestStatus::Inactive;
}

FString UPISQuestComponent::GetStage(const FString& QuestId) const
{
    if (const FPISQuestState* State = FindQuestState(States, QuestId))
    {
        return State->StageId;
    }
    return FString();
}

void UPISQuestComponent::SetProgress(const FString& QuestId, int32 Progress)
{
    if (FPISQuestState* State = FindQuestState(States, QuestId))
    {
        State->ProgressCount = FMath::Max(0, Progress);
        OnQuestChanged.Broadcast(*State);
    }
}

void UPISQuestComponent::AddProgress(const FString& QuestId, int32 Delta)
{
    if (FPISQuestState* State = FindQuestState(States, QuestId))
    {
        State->ProgressCount = FMath::Max(0, State->ProgressCount + Delta);
        OnQuestChanged.Broadcast(*State);
    }
}

void UPISQuestComponent::ResetAll()
{
    States.Reset();
}

void UPISQuestComponent::LockQuest(const FString& QuestId)
{
    if (FindQuestState(States, QuestId) == nullptr)
    {
        FPISQuestState State;
        State.QuestId = QuestId;
        State.StageId = TEXT("locked");
        State.Status = EPISQuestStatus::Locked;
        States.Add(State);
    }
}

bool UPISQuestComponent::IsFactionBlocked(const FString& QuestId) const
{
    // Heuristic by quest id prefix - canonical quest ids are old_trial_*, new_trial_*.
    const bool bOld = QuestId.StartsWith(TEXT("old_trial"));
    const bool bNew = QuestId.StartsWith(TEXT("new_trial"));
    if (!bOld && !bNew) { return false; }
    for (const FPISQuestState& S : States)
    {
        if (S.StageId == TEXT("done") && S.Status == EPISQuestStatus::Completed)
        {
            const bool bCompletedOld = S.QuestId.StartsWith(TEXT("old_trial"));
            const bool bCompletedNew = S.QuestId.StartsWith(TEXT("new_trial"));
            if (bCompletedOld && bNew) { return true; }
            if (bCompletedNew && bOld) { return true; }
        }
    }
    return false;
}
