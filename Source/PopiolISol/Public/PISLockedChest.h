// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PISLockedChest.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPISInventoryComponent;

/** Three-step L/R lock. Wrong turn breaks one lockpick; skill rank gates difficulty. */
UCLASS(Blueprintable)
class POPIOLISOL_API APISLockedChest : public AActor
{
	GENERATED_BODY()

public:
	APISLockedChest();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest")
	FString ChestId = TEXT("chest_prototype");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest")
	FString LootTableId = TEXT("loot_prototype_chest");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest", meta = (ClampMin = "1", ClampMax = "3"))
	int32 RequiredLockRank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest")
	FString Sequence = TEXT("LRL");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	bool bOpened = false;

	UFUNCTION(BlueprintCallable, Category = "Chest")
	bool BeginLockpick(int32 PlayerRank);

	UFUNCTION(BlueprintCallable, Category = "Chest")
	bool SubmitLockTurn(bool bRight, UPISInventoryComponent* Inventory);

	UFUNCTION(BlueprintCallable, Category = "Chest")
	bool OpenWithKey(const FString& KeyId);

	UFUNCTION(BlueprintPure, Category = "Chest")
	FString Prompt() const;

private:
	int32 Cursor = 0;
	bool bPicking = false;
};
