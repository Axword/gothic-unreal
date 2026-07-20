// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Dom/JsonObject.h"
#include "PISJsonDataSubsystem.generated.h"

/** Loads canonical design data from Content/Data/Json. Runtime state is never written here. */
UCLASS()
class POPIOLISOL_API UPISJsonDataSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Data")
    bool HasId(const FString& Id) const;

    UFUNCTION(BlueprintCallable, Category = "Data")
    FString GetValidationReport() const { return ValidationReport; }

    UFUNCTION(BlueprintCallable, Category = "Data")
    bool ReloadAndValidate();

    const TSharedPtr<FJsonObject>* Find(const FString& Id) const { return Records.Find(Id); }

    /** Iterating accessor for NPC markers and similar flows. */
    const TArray<TSharedPtr<FJsonValue>>& GetAllRecords() const { return RecordsView; }

private:
    TMap<FString, TSharedPtr<FJsonObject>> Records;
    TArray<TSharedPtr<FJsonValue>> RecordsView;
    FString ValidationReport;

    bool LoadFile(const FString& Filename, TArray<FString>& Errors);
};
