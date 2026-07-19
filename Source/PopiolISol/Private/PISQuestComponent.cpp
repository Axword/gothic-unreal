#include "PISQuestComponent.h"
static FPISQuestState* Q(TArray<FPISQuestState>& A,const FString& Id){for(auto& X:A)if(X.QuestId==Id)return &X;return nullptr;}
bool UPISQuestComponent::StartQuest(const FString& Id,const FString& Stage){
 if(Id.IsEmpty()||IsQuestLocked(Id))return false;
 FPISQuestState* Existing=Q(States,Id);
 if(Existing&&Existing->Status!=EPISQuestStatus::Inactive)return false;
 if(Existing){Existing->StageId=Stage;Existing->Status=EPISQuestStatus::Active;OnQuestChanged.Broadcast(*Existing);return true;}
 FPISQuestState S;S.QuestId=Id;S.StageId=Stage;S.Status=EPISQuestStatus::Active;States.Add(S);OnQuestChanged.Broadcast(S);return true;
}
bool UPISQuestComponent::SetStage(const FString& Id,const FString& Stage){
 FPISQuestState*S=Q(States,Id);
 if(!S||S->Status!=EPISQuestStatus::Active||Stage.IsEmpty())return false;
 S->StageId=Stage;OnQuestChanged.Broadcast(*S);return true;
}
bool UPISQuestComponent::FinishQuest(const FString& Id,bool bOK){
 FPISQuestState*S=Q(States,Id);
 if(!S||S->Status!=EPISQuestStatus::Active)return false;
 S->StageId=bOK?TEXT("done"):TEXT("failed");
 S->Status=bOK?EPISQuestStatus::Completed:EPISQuestStatus::Failed;
 OnQuestChanged.Broadcast(*S);return true;
}
EPISQuestStatus UPISQuestComponent::GetStatus(const FString& Id)const{
 for(const auto&S:States)if(S.QuestId==Id)return S.Status;
 return EPISQuestStatus::Inactive;
}
bool UPISQuestComponent::LockQuest(const FString& Id){
 if(Id.IsEmpty())return false;
 FPISQuestState*S=Q(States,Id);
 if(S){
  if(S->Status==EPISQuestStatus::Completed)return false;
  S->Status=EPISQuestStatus::Locked;S->StageId=TEXT("locked");OnQuestChanged.Broadcast(*S);return true;
 }
 FPISQuestState NS;NS.QuestId=Id;NS.StageId=TEXT("locked");NS.Status=EPISQuestStatus::Locked;States.Add(NS);OnQuestChanged.Broadcast(NS);return true;
}
bool UPISQuestComponent::IsQuestLocked(const FString& Id)const{
 return GetStatus(Id)==EPISQuestStatus::Locked;
}
bool UPISQuestComponent::ChooseFaction(const FString& FactionId){
 if(!ActiveFaction.IsEmpty()||FactionId.IsEmpty())return false;
 if(FactionId!=TEXT("stary_porzadek")&&FactionId!=TEXT("nowy_porzadek"))return false;
 ActiveFaction=FactionId;
 if(ActiveFaction==TEXT("stary_porzadek")){
  for(int32 i=1;i<=5;++i) LockQuest(FString::Printf(TEXT("new_trial_%d"),i));
 } else {
  for(int32 i=1;i<=5;++i) LockQuest(FString::Printf(TEXT("old_trial_%d"),i));
 }
 return true;
}
