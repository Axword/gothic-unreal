#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PISInventoryComponent.generated.h"
USTRUCT(BlueprintType) struct FPISInventoryStack { GENERATED_BODY()
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FString ItemId;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Count=0;
};
/** ID-only inventory. Quantities are resolved against the JSON catalog at presentation time. */
UCLASS(ClassGroup=(PIS), meta=(BlueprintSpawnableComponent)) class POPIOLISOL_API UPISInventoryComponent:public UActorComponent { GENERATED_BODY()
public:
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TArray<FPISInventoryStack> Items;
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly) FString EquippedWeaponId;
 UFUNCTION(BlueprintCallable) bool AddItem(const FString& ItemId,int32 Count=1);
 UFUNCTION(BlueprintCallable) bool RemoveItem(const FString& ItemId,int32 Count=1);
 UFUNCTION(BlueprintPure) int32 CountItem(const FString& ItemId)const;
 UFUNCTION(BlueprintCallable) bool EquipWeapon(const FString& ItemId);
 UFUNCTION(BlueprintCallable) bool EquipWeaponWithStats(const FString& ItemId,int32 PlayerStrength,int32 PlayerDexterity,int32 RequiredStrength,int32 RequiredDexterity);
};
