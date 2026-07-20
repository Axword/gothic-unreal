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
	if (QuestId.IsEmpty() || FindQuestState(States, QuestId) != nullptr)
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
