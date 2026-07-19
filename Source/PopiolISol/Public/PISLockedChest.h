#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PISLockedChest.generated.h"
class UBoxComponent; class UStaticMeshComponent; class UPISInventoryComponent;
/** Three-step L/R lock. Wrong turn breaks one lockpick; skill rank gates difficulty. */
UCLASS(Blueprintable) class POPIOLISOL_API APISLockedChest:public AActor { GENERATED_BODY() public:
 APISLockedChest();
 UPROPERTY(VisibleAnywhere) UBoxComponent* Trigger;
 UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh;
 UPROPERTY(EditAnywhere,BlueprintReadOnly) FString ChestId=TEXT("chest_prototype");
 UPROPERTY(EditAnywhere,BlueprintReadOnly) FString LootTableId=TEXT("loot_prototype_chest");
 UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin=1,ClampMax=3)) int32 RequiredLockRank=1;
 UPROPERTY(EditAnywhere,BlueprintReadOnly) FString Sequence=TEXT("LRL");
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly) bool bOpened=false;
 UFUNCTION(BlueprintCallable) bool BeginLockpick(int32 PlayerRank);
 UFUNCTION(BlueprintCallable) bool SubmitLockTurn(bool bRight,UPISInventoryComponent* Inventory);
 UFUNCTION(BlueprintCallable) bool OpenWithKey(const FString& KeyId);
 UFUNCTION(BlueprintPure) FString Prompt()const;
private: int32 Cursor=0; bool bPicking=false;
};
