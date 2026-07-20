// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PISSaveGameSubsystem.generated.h"

class APawn;
class UPISStatsComponent;
class UPISInventoryComponent;
class UPISQuestComponent;

USTRUCT(BlueprintType)
struct FPISSaveSlot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) FString SlotName;
    UPROPERTY(BlueprintReadOnly) FString Timestamp;
    UPROPERTY(BlueprintReadOnly) FString SummaryLocation;
    UPROPERTY(BlueprintReadOnly) int32 PlayerLevel = 1;
    UPROPERTY(BlueprintReadOnly) FString FactionChoice;
};

UCLASS()
class POPIOLISOL_API UPISSaveGameSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static constexpr int32 SchemaVersion = 1;

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool SaveSlot(const FString& SlotName, APawn* Player);

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadSlot(const FString& SlotName, APawn* Player);

    UFUNCTION(BlueprintCallable, Category = "Save")
    TArray<FPISSaveSlot> ListSlots() const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool DeleteSlot(const FString& SlotName);

    /** Returns the absolute save directory (Saved/SaveGames). */
    FString GetSaveDirectory() const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    FString GetLastError() const { return LastError; }

private:
    FString LastError;
    static FString SerializePlayerToJson(APawn* Player, FString& OutError);
    static bool ApplyJsonToPlayer(APawn* Player, const FString& Json, FString& OutError);
};
