// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PISDialogueComponent.generated.h"

USTRUCT(BlueprintType)
struct FPISDialogueChoice
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") FString Id;
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") FString Text;
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") FString Next;
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") bool bIsExit = false;
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") bool bEnabled = true;
};

USTRUCT(BlueprintType)
struct FPISDialogueNodeView
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") FString Id;
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") FString Speaker;
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") FString Text;
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue") TArray<FPISDialogueChoice> Choices;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPISDialogueFinished, FString, DialogueId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPISDialogueOpened, FString, DialogueId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPISDialogueNodeShown, const FPISDialogueNodeView&, Node);

UCLASS(ClassGroup = (PIS), meta = (BlueprintSpawnableComponent))
class POPIOLISOL_API UPISDialogueComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Dialogue") FPISDialogueOpened OnOpened;
    UPROPERTY(BlueprintAssignable, Category = "Dialogue") FPISDialogueNodeShown OnNodeShown;
    UPROPERTY(BlueprintAssignable, Category = "Dialogue") FPISDialogueFinished OnFinished;

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool OpenDialogue(const FString& DialogueId);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void CloseDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool Choose(const FString& ChoiceId);

    UFUNCTION(BlueprintPure, Category = "Dialogue")
    bool IsOpen() const { return CurrentDialogueId.Len() > 0; }

    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FPISDialogueNodeView GetCurrentNode() const { return CurrentNode; }

private:
    FString CurrentDialogueId;
    FString CurrentNodeId;
    FPISDialogueNodeView CurrentNode;

    /** Stable, mutable set of boolean flags (e.g. "biel_trusted", "faction_choice:stary_porzadek"). */
    UPROPERTY()
    TSet<FString> Flags;

    /** Skill/stat hints resolved against the player at runtime. Filled by ApplyCondition. */
    bool EvaluateCondition(const TSharedPtr<class FJsonObject>& CondObj) const;
    void RunAction(const TSharedPtr<class FJsonObject>& ActionObj);
    void BuildNodeView(const FString& NodeId, const TSharedPtr<class FJsonObject>& NodeObj);

    /** Helpers to broadcast after a node change. */
    void ShowNodeById(const FString& NodeId);
    void FinishDialogue();

    // Weak resolve of player reference; set by GameMode on dialogue start.
    UPROPERTY() TWeakObjectPtr<AActor> CachedPlayer;
};
