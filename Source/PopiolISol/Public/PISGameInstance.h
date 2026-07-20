// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PISGameInstance.generated.h"

class UPISJsonDataSubsystem;
class UPISSaveGameSubsystem;

UCLASS()
class POPIOLISOL_API APISGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintPure, Category = "PIS")
    UPISJsonDataSubsystem* GetData() const;

    UFUNCTION(BlueprintPure, Category = "PIS")
    UPISSaveGameSubsystem* GetSaves() const;
};
