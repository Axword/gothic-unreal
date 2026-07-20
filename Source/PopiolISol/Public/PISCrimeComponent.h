// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PISCrimeComponent.generated.h"

UENUM(BlueprintType)
enum class EPISCrime : uint8
{
    Trespass,
    Theft,
    Lockpick,
    Assault
};

UENUM(BlueprintType)
enum class EPISCrimeReaction : uint8
{
    None,
    Warning,
    Demand,
    Alarm,
    Attack
};

USTRUCT(BlueprintType)
struct FPISWitnessReport
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Crime") FString WitnessId;
    UPROPERTY(BlueprintReadOnly, Category = "Crime") EPISCrimeReaction Reaction = EPISCrimeReaction::None;
    UPROPERTY(BlueprintReadOnly, Category = "Crime") float Distance = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPISCrimeEscalated, const FPISWitnessReport&, Report);

UCLASS(ClassGroup = (PIS), meta = (BlueprintSpawnableComponent))
class POPIOLISOL_API UPISCrimeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Distance at which a witness can hear loud actions (combat, lockpick). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    float WitnessHearRadius = 1200.f;

    /** Distance at which a witness can see a clear action (theft, assault). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crime")
    float WitnessSeeRadius = 1800.f;

    UPROPERTY(BlueprintAssignable, Category = "Crime")
    FPISCrimeEscalated OnEscalation;

    UFUNCTION(BlueprintCallable, Category = "Crime")
    TArray<FPISWitnessReport> ReportCrime(AActor* Instigator, FVector Location, EPISCrime Crime, EPISCrimeReaction MaxReaction) const;

    /** Apply an escalation to a single NPC actor (calls a BlueprintImplementableEvent to drive dialogue/combat). */
    UFUNCTION(BlueprintCallable, Category = "Crime")
    void EscalateWitness(AActor* NpcActor, EPISCrime Crime, EPISCrimeReaction Reaction);

    UFUNCTION(BlueprintPure, Category = "Crime")
    static EPISCrimeReaction ReactionForFaction(const FString& Faction, EPISCrime Crime, int32 Reputation);
};
