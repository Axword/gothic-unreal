// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISMonsterController.h"
#include "PISMonster.h"
#include "PISCharacter.h"
#include "PISWorldClock.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

APISMonsterController::APISMonsterController()
{
    PrimaryActorTick.bCanEverTick = true;
    bWantsPlayerState = false;

    Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
    Sight->SightRadius = 1400.f;
    Sight->LoseSightRadius = 2200.f;
    Sight->PeripheralVisionAngleDegrees = 90.f;
    Sight->DetectionByAffiliation.bDetectEnemies = true;
    Sight->DetectionByAffiliation.bDetectNeutrals = true;
    Sight->DetectionByAffiliation.bDetectFriendlies = true;
    Perception->ConfigureSense(*Sight);
    Perception->SetDominantSense(Sight->GetSenseImplementation());

    Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing"));
    Hearing->HearingRange = 1200.f;
    Hearing->DetectionByAffiliation.bDetectEnemies = true;
    Hearing->DetectionByAffiliation.bDetectNeutrals = true;
    Hearing->DetectionByAffiliation.bDetectFriendlies = true;
    Perception->ConfigureSense(*Hearing);
}

void APISMonsterController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    Controlled = Cast<APISMonster>(InPawn);
    if (Controlled)
    {
        Sight->SightRadius = Controlled->SightRadius;
        Sight->LoseSightRadius = Controlled->LoseRadius;
    }
}

void APISMonsterController::OnHitReact(FVector HitFrom, AActor* Instigator)
{
    if (!Controlled) { return; }
    if (APawn* P = Cast<APawn>(Instigator))
    {
        Engage(P);
    }
    Controlled->SetState(EPISMonsterState::Chasing);
    Controlled->SetActorRotation((HitFrom - Controlled->GetActorLocation()).GetSafeNormal2D().Rotation());
}

void APISMonsterController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    EvaluateSenses(DeltaSeconds);
}

void APISMonsterController::EvaluateSenses(float DeltaSeconds)
{
    if (!Controlled || Controlled->IsDead()) { return; }

    // Night aggression for wild monsters.
    UWorld* World = GetWorld();
    APISWorldClock* Clock = nullptr;
    for (TActorIterator<APISWorldClock> It(World); It; ++It) { Clock = *It; break; }
    const bool bNight = Clock && Clock->IsNight();

    AActor* Best = nullptr;
    float BestDistSq = TNumericLimits<float>::Max();
    for (TActorIterator<APawn> It(World); It; ++It)
    {
        APawn* P = *It;
        if (!P || P == Controlled) { continue; }
        if (!P->IsA(APISCharacter::StaticClass())) { continue; }
        if (P->IsPendingKillPending()) { continue; }
        const float D2 = FVector::DistSquared(P->GetActorLocation(), Controlled->GetActorLocation());
        if (D2 < BestDistSq) { BestDistSq = D2; Best = P; }
    }

    const float SightR = Controlled->SightRadius;
    const bool bInSight = Best && BestDistSq < SightR * SightR;
    if (bInSight)
    {
        Engage(Cast<APawn>(Best));
        TimeSinceSawTarget = 0.f;
    }
    else if (bHasTarget)
    {
        TimeSinceSawTarget += DeltaSeconds;
        if (TimeSinceSawTarget > LoseInterestAfter)
        {
            Disengage();
        }
    }

    if (bHasTarget && Controlled->State == EPISMonsterState::Chasing)
    {
        if (BestDistSq < Controlled->AttackRange * Controlled->AttackRange)
        {
            Controlled->SetState(EPISMonsterState::Attacking);
        }
    }

    // If wild and night-only aggression is set, prefer patrolling during day.
    if (Controlled->FactionId == TEXT("wild") && Controlled->bAggressiveAtNight && !bNight && !bHasTarget)
    {
        Controlled->SetState(EPISMonsterState::Idle);
    }
}

void APISMonsterController::Engage(APawn* Target)
{
    if (!Target || !Controlled) { return; }
    bHasTarget = true;
    if (Cast<APISCharacter>(Target) && !Target->ActorHasTag(TEXT("PIS_Hostile")))
    {
        Target->Tags.AddUnique(TEXT("PIS_Hostile"));
    }
    MoveToActor(Target, Controlled->AttackRange * 0.5f, true, true, false);
}

void APISMonsterController::Disengage()
{
    bHasTarget = false;
    if (Controlled) { Controlled->SetState(EPISMonsterState::Returning); }
    ReturnToPatrol();
}

void APISMonsterController::ReturnToPatrol()
{
    if (!Controlled) { return; }
    UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!Nav) { return; }
    Nav->SimpleMoveToLocation(this, Controlled->PatrolOrigin);
    Controlled->SetState(EPISMonsterState::Patrolling);
}

ETeamAttitude::Type APISMonsterController::GetTeamAttitudeTowards(const AActor& Other) const
{
    if (const APISMonster* M = Cast<APISMonster>(&Other))
    {
        if (M->FactionId == Controlled->FactionId) { return ETeamAttitude::Friendly; }
    }
    if (Cast<APISCharacter>(&Other)) { return ETeamAttitude::Hostile; }
    return ETeamAttitude::Neutral;
}
