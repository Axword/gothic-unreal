// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PISNPCController.generated.h"

class APISNPC;
class APISCharacter;

UCLASS()
class POPIOLISOL_API APISNPCController : public AAIController
{
    GENERATED_BODY()

public:
    APISNPCController();

    UPROPERTY(BlueprintReadOnly, Category = "PIS") APISNPC* Controlled = nullptr;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") FString CurrentActivity;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") FString CurrentMarker;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") int32 LastScheduleDay = -1;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") FString CurrentSlotId;

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void RefreshSchedule();

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void OnCrimeWitnessed(FVector At, FString WitnessId);

    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;
};
