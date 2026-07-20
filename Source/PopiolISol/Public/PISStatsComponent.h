// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PISStatsComponent.generated.h"

UENUM(BlueprintType)
enum class EPISFaction : uint8
{
    None,
    StaryPorzadek, // Rygiel
    NowyBrzeg,     // Wolny Brzeg
    Neutralny
};

USTRUCT(BlueprintType)
struct FPISPrimaryStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 Strength = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 Dexterity = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 Vitality = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 ManaStat = 5;
};

USTRUCT(BlueprintType)
struct FPISSkillRanks
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 Sword = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 Bow = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 Lockpick = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 Steal = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 Skinning = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 FireMagic = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 IceMagic = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPISStatsChanged, int32, NewLevel, int32, NewXp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPISReputationChanged, EPISFaction, Faction);

UCLASS(ClassGroup = (PIS), meta = (BlueprintSpawnableComponent))
class POPIOLISOL_API UPISStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") int32 Level = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") int32 Xp = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") int32 LearningPoints = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") int32 CurrentHp = 80;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") int32 CurrentMana = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") FPISPrimaryStats Primary;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") FPISSkillRanks Skills;

    /** Reputation values per faction id (stary_porzadek / nowy_brzeg). -100..100. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") TMap<FString, int32> Reputation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") FString FactionChoice;

    UPROPERTY(BlueprintAssignable, Category = "Stats") FPISStatsChanged OnStatsChanged;
    UPROPERTY(BlueprintAssignable, Category = "Stats") FPISReputationChanged OnReputationChanged;

    UFUNCTION(BlueprintPure, Category = "Stats") int32 GetMaxHp() const;
    UFUNCTION(BlueprintPure, Category = "Stats") int32 GetMaxMana() const;
    UFUNCTION(BlueprintPure, Category = "Stats") int32 GetXpForNextLevel() const;
    UFUNCTION(BlueprintPure, Category = "Stats") int32 GetReputation(EPISFaction Faction) const;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void AwardXp(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool LearnSkill(FName SkillId, int32 Cost = 1);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ModifyReputation(EPISFaction Faction, int32 Delta);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ApplyDamage(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool ConsumeMana(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void RestoreHp(int32 Amount) { CurrentHp = FMath::Min(GetMaxHp(), CurrentHp + Amount); }
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void RestoreMana(int32 Amount) { CurrentMana = FMath::Min(GetMaxMana(), CurrentMana + Amount); }
};
