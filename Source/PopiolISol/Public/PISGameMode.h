// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PISGameMode.generated.h"

UCLASS()
class POPIOLISOL_API APISGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    APISGameMode();

    /** Resets the player to a safe spawn (used by Load and by Knockout recovery). */
    UFUNCTION(BlueprintCallable, Category = "PIS")
    void RespawnPlayer();

    /** Knockout the player: zero HP -> teleport to last marker + restore HP. */
    UFUNCTION(BlueprintCallable, Category = "PIS")
    void KnockoutPlayer();

    virtual void BeginPlay() override;
};
