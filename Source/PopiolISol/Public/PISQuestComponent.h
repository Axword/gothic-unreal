#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PISQuestComponent.generated.h"
UENUM(BlueprintType) enum class EPISQuestStatus:uint8 { Inactive,Active,Completed,Failed,Locked };
USTRUCT(BlueprintType) struct FPISQuestState { GENERATED_BODY() UPROPERTY(EditAnywhere,BlueprintReadWrite) FString QuestId; UPROPERTY(EditAnywhere,BlueprintReadWrite) FString StageId; UPROPERTY(EditAnywhere,BlueprintReadWrite) EPISQuestStatus Status=EPISQuestStatus::Inactive;};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPISQuestChanged,const FPISQuestState&, State);
UCLASS(ClassGroup=(PIS),meta=(BlueprintSpawnableComponent)) class POPIOLISOL_API UPISQuestComponent:public UActorComponent { GENERATED_BODY() public:
 UPROPERTY(BlueprintAssignable) FPISQuestChanged OnQuestChanged; UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TArray<FPISQuestState> States;
 UFUNCTION(BlueprintCallable) bool StartQuest(const FString& QuestId,const FString& StartStage=TEXT("start"));
 UFUNCTION(BlueprintCallable) bool SetStage(const FString& QuestId,const FString& StageId);
 UFUNCTION(BlueprintCallable) bool FinishQuest(const FString& QuestId,bool bSucceeded=true);
 UFUNCTION(BlueprintPure) EPISQuestStatus GetStatus(const FString& QuestId)const;
};
