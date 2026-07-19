#include "PISQuestComponent.h"
static FPISQuestState* Q(TArray<FPISQuestState>& A,const FString& Id){for(auto& X:A)if(X.QuestId==Id)return &X;return nullptr;}
bool UPISQuestComponent::StartQuest(const FString& Id,const FString& Stage){if(Id.IsEmpty()||Q(States,Id))return false;FPISQuestState S;S.QuestId=Id;S.StageId=Stage;S.Status=EPISQuestStatus::Active;States.Add(S);OnQuestChanged.Broadcast(S);return true;}
bool UPISQuestComponent::SetStage(const FString& Id,const FString& Stage){FPISQuestState*S=Q(States,Id);if(!S||S->Status!=EPISQuestStatus::Active||Stage.IsEmpty())return false;S->StageId=Stage;OnQuestChanged.Broadcast(*S);return true;}
bool UPISQuestComponent::FinishQuest(const FString& Id,bool bOK){FPISQuestState*S=Q(States,Id);if(!S||S->Status!=EPISQuestStatus::Active)return false;S->StageId=bOK?TEXT("done"):TEXT("failed");S->Status=bOK?EPISQuestStatus::Completed:EPISQuestStatus::Failed;OnQuestChanged.Broadcast(*S);return true;}
EPISQuestStatus UPISQuestComponent::GetStatus(const FString& Id)const{for(const auto&S:States)if(S.QuestId==Id)return S.Status;return EPISQuestStatus::Inactive;}
