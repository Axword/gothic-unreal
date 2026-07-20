// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISGameInstance.h"
#include "PISJsonDataSubsystem.h"
#include "PISSaveGameSubsystem.h"

void APISGameInstance::Init()
{
    Super::Init();
    // Subsystems auto-initialise; we just need to make sure they exist.
    GetData();
    GetSaves();
}

UPISJsonDataSubsystem* APISGameInstance::GetData() const
{
    return GetSubsystem<UPISJsonDataSubsystem>();
}

UPISSaveGameSubsystem* APISGameInstance::GetSaves() const
{
    return GetSubsystem<UPISSaveGameSubsystem>();
}
