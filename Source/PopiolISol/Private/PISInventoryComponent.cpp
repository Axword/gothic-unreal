#include "PISInventoryComponent.h"
bool UPISInventoryComponent::AddItem(const FString& Id,int32 N){if(Id.IsEmpty()||N<=0)return false;for(auto&S:Items)if(S.ItemId==Id){S.Count+=N;return true;} FPISInventoryStack S;S.ItemId=Id;S.Count=N;Items.Add(S);return true;}
bool UPISInventoryComponent::RemoveItem(const FString& Id,int32 N){if(N<=0)return false;for(int32 i=0;i<Items.Num();++i)if(Items[i].ItemId==Id){if(Items[i].Count<N)return false;Items[i].Count-=N;if(!Items[i].Count)Items.RemoveAt(i);if(EquippedWeaponId==Id&&CountItem(Id)==0)EquippedWeaponId.Empty();return true;}return false;}
int32 UPISInventoryComponent::CountItem(const FString& Id)const{for(const auto&S:Items)if(S.ItemId==Id)return S.Count;return 0;}
bool UPISInventoryComponent::EquipWeapon(const FString& Id){if(CountItem(Id)<1)return false;EquippedWeaponId=Id;return true;}
