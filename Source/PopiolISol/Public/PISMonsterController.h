// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PISMonsterController.generated.h"

class APISMonster;

UCLASS()
class POPIOLISOL_API APISMonsterController : public AAIController
{
    GENERATED_BODY()

public:
    APISMonsterController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") float LoseInterestAfter = 8.f;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") APISMonster* Controlled = nullptr;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") float TimeSinceSawTarget = 0.f;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") bool bHasTarget = false;

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void OnHitReact(FVector HitFrom, AActor* Instigator);

    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
    void EvaluateSenses(float DeltaSeconds);
    void Engage(APawn* Target);
    void Disengage();
    void ReturnToPatrol();
};
