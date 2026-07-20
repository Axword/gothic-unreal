// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISCrimeComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

TArray<FPISWitnessReport> UPISCrimeComponent::ReportCrime(AActor* Instigator, FVector Location, EPISCrime Crime, EPISCrimeReaction MaxReaction) const
{
    TArray<FPISWitnessReport> Reports;
    UWorld* World = GetWorld();
    if (!World) { return Reports; }

    const float MaxDist = (Crime == EPISCrime::Theft || Crime == EPISCrime::Assault)
        ? WitnessSeeRadius
        : WitnessHearRadius;

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Other = *It;
        if (!Other || Other == Instigator) { continue; }
        // Restrict to NPCs only - we look for a tag or a CrimeWitness component later.
        // For now treat all pawns as potential witnesses.
        if (!Other->IsA(APawn::StaticClass())) { continue; }

        const float D = FVector::Dist(Other->GetActorLocation(), Location);
        if (D > MaxDist) { continue; }

        FPISWitnessReport R;
        R.WitnessId = Other->GetName();
        R.Distance = D;
        R.Reaction = MaxReaction;
        Reports.Add(R);
    }
    return Reports;
}

void UPISCrimeComponent::EscalateWitness(AActor* NpcActor, EPISCrime Crime, EPISCrimeReaction Reaction)
{
    if (!NpcActor) { return; }
    FPISWitnessReport R;
    R.WitnessId = NpcActor->GetName();
    R.Reaction = Reaction;
    R.Distance = 0.f;
    OnEscalation.Broadcast(R);

    if (Reaction == EPISCrimeReaction::Alarm || Reaction == EPISCrimeReaction::Attack)
    {
        // Aggressive: tell AI to attack. Hook implemented by NPC AI controller.
        NpcActor->Tags.AddUnique(TEXT("PIS_CombatTarget"));
    }
    else if (Reaction == EPISCrimeReaction::Warning)
    {
        NpcActor->Tags.AddUnique(TEXT("PIS_Warned"));
    }
    else if (Reaction == EPISCrimeReaction::Demand)
    {
        NpcActor->Tags.AddUnique(TEXT("PIS_Demanded"));
    }
}

EPISCrimeReaction UPISCrimeComponent::ReactionForFaction(const FString& Faction, EPISCrime Crime, int32 Reputation)
{
    // Lighter response for allies, harsher for enemies.
    const bool bAlly = Reputation >= 25;
    const bool bEnemy = Reputation <= -25;
    if (Crime == EPISCrime::Theft)
    {
        if (bAlly) { return EPISCrimeReaction::Warning; }
        if (bEnemy) { return EPISCrimeReaction::Attack; }
        return EPISCrimeReaction::Demand;
    }
    if (Crime == EPISCrime::Assault)
    {
        if (bAlly) { return EPISCrimeReaction::Demand; }
        return EPISCrimeReaction::Attack;
    }
    if (Crime == EPISCrime::Trespass) { return bEnemy ? EPISCrimeReaction::Alarm : EPISCrimeReaction::Warning; }
    if (Crime == EPISCrime::Lockpick) { return bAlly ? EPISCrimeReaction::None : EPISCrimeReaction::Warning; }
    return EPISCrimeReaction::None;
}
