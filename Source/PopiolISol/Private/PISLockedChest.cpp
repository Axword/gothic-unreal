#include "PISLockedChest.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PISInventoryComponent.h"
APISLockedChest::APISLockedChest(){RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));Trigger->SetupAttachment(RootComponent);Trigger->SetBoxExtent(FVector(80));Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));Mesh->SetupAttachment(RootComponent);}
bool APISLockedChest::BeginLockpick(int32 Rank){if(bOpened||Rank<RequiredLockRank)return false;Cursor=0;bPicking=true;return true;}
bool APISLockedChest::BeginLockpickWithInventory(int32 Rank,UPISInventoryComponent* I){if(bOpened||Rank<RequiredLockRank||!I||I->CountItem(TEXT("lockpick"))<1)return false;Cursor=0;bPicking=true;return true;}
bool APISLockedChest::SubmitLockTurn(bool Right,UPISInventoryComponent* I){
 if(!bPicking||bOpened||!I||I->CountItem(TEXT("lockpick"))<1){bPicking=false;return false;}
 const TCHAR Expected=Sequence[Cursor];
 if((Right&&Expected=='R')||(!Right&&Expected=='L')){
  if(++Cursor>=Sequence.Len()){bOpened=true;bPicking=false;}
  return true;
 }
 I->RemoveItem(TEXT("lockpick"),1);Cursor=0;
 if(I->CountItem(TEXT("lockpick"))<1) bPicking=false;
 return false;
}bool APISLockedChest::OpenWithKey(const FString& Key){if(bOpened||Key!=TEXT("key_watch"))return false;bOpened=true;bPicking=false;return true;}FString APISLockedChest::Prompt()const{return bOpened?TEXT("Przeszukaj skrzynię"):FString::Printf(TEXT("Zamek %d: %s"),RequiredLockRank,*ChestId);}
