// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PISQuestComponent.generated.h"

UENUM(BlueprintType)
enum class EPISQuestStatus : uint8
{
    Inactive,
    Active,
    Completed,
    Failed,
    Locked
};

USTRUCT(BlueprintType)
struct FPISQuestState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString QuestId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FString StageId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    EPISQuestStatus Status = EPISQuestStatus::Inactive;

    /** Custom integer counter for "kill N", "collect N" etc. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 ProgressCount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPISQuestChanged, const FPISQuestState&, State);

UCLASS(ClassGroup = (PIS), meta = (BlueprintSpawnableComponent))
class POPIOLISOL_API UPISQuestComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FPISQuestChanged OnQuestChanged;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
    TArray<FPISQuestState> States;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool StartQuest(const FString& QuestId, const FString& StartStage = TEXT("start"));

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool SetStage(const FString& QuestId, const FString& StageId);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool FinishQuest(const FString& QuestId, bool bSucceeded = true);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool FailQuest(const FString& QuestId) { return FinishQuest(QuestId, false); }

    UFUNCTION(BlueprintPure, Category = "Quest")
    EPISQuestStatus GetStatus(const FString& QuestId) const;

    UFUNCTION(BlueprintPure, Category = "Quest")
    FString GetStage(const FString& QuestId) const;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetProgress(const FString& QuestId, int32 Progress);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void AddProgress(const FString& QuestId, int32 Delta = 1);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void ResetAll();

    /** Mark a quest id as locked so future StartQuest refuses. Used by faction choice. */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void LockQuest(const FString& QuestId);

    /** Returns true if the player has chosen a faction opposite to QuestId's faction. */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool IsFactionBlocked(const FString& QuestId) const;
};
