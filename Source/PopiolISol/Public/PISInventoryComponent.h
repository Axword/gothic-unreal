// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PISInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FPISInventoryStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Count = 0;
};

/** ID-only inventory. Quantities are resolved against the JSON catalog at presentation time. */
UCLASS(ClassGroup = (PIS), meta = (BlueprintSpawnableComponent))
class POPIOLISOL_API UPISInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FPISInventoryStack> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	FString EquippedWeaponId;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FString& ItemId, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(const FString& ItemId, int32 Count = 1);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 CountItem(const FString& ItemId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool EquipWeapon(const FString& ItemId);
};
