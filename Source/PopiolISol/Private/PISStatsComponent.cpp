// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISStatsComponent.h"

int32 UPISStatsComponent::GetMaxHp() const
{
    // HP = 80 + 12 * (level - 1) + 4 * vitality. Matches UPISProgressionLibrary::MaxHpForLevel.
    return 80 + (Level - 1) * 12 + Primary.Vitality * 4;
}

int32 UPISStatsComponent::GetMaxMana() const
{
    return 30 + Level * 10 + Primary.ManaStat * 5;
}

int32 UPISStatsComponent::GetXpForNextLevel() const
{
    // 100 + 75 * level
    return 100 + Level * 75;
}

int32 UPISStatsComponent::GetReputation(EPISFaction Faction) const
{
    const FString Key = Faction == EPISFaction::StaryPorzadek ? TEXT("stary_porzadek")
                  : Faction == EPISFaction::NowyBrzeg      ? TEXT("nowy_brzeg")
                  : Faction == EPISFaction::Neutralny      ? TEXT("neutralny")
                  : TEXT("none");
    if (const int32* Value = Reputation.Find(Key))
    {
        return *Value;
    }
    return 0;
}

void UPISStatsComponent::AwardXp(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }
    Xp += Amount;
    // Cap levels reasonably so the slice remains playable.
    while (Xp >= GetXpForNextLevel() && Level < 50)
    {
        Xp -= GetXpForNextLevel();
        ++Level;
        LearningPoints += 10;
        CurrentHp = GetMaxHp();
        CurrentMana = GetMaxMana();
    }
    OnStatsChanged.Broadcast(Level, Xp);
}

bool UPISStatsComponent::LearnSkill(FName SkillId, int32 Cost)
{
    if (LearningPoints < Cost)
    {
        return false;
    }
    auto Inc = [&](int32& Rank)
    {
        if (Rank >= 10) { return false; }
        ++Rank;
        LearningPoints -= Cost;
        OnStatsChanged.Broadcast(Level, Xp);
        return true;
    };

    const FString N = SkillId.ToString().ToLower();
    if (N == TEXT("sword"))    return Inc(Skills.Sword);
    if (N == TEXT("bow"))      return Inc(Skills.Bow);
    if (N == TEXT("lockpick")) return Inc(Skills.Lockpick);
    if (N == TEXT("steal"))    return Inc(Skills.Steal);
    if (N == TEXT("skinning")) return Inc(Skills.Skinning);
    if (N == TEXT("fire"))     return Inc(Skills.FireMagic);
    if (N == TEXT("ice"))      return Inc(Skills.IceMagic);
    return false;
}

void UPISStatsComponent::ModifyReputation(EPISFaction Faction, int32 Delta)
{
    const FString Key = Faction == EPISFaction::StaryPorzadek ? TEXT("stary_porzadek")
                  : Faction == EPISFaction::NowyBrzeg      ? TEXT("nowy_brzeg")
                  : Faction == EPISFaction::Neutralny      ? TEXT("neutralny")
                  : TEXT("none");
    int32& Value = Reputation.FindOrAdd(Key);
    Value = FMath::Clamp(Value + Delta, -100, 100);
    OnReputationChanged.Broadcast(Faction);
}

void UPISStatsComponent::ApplyDamage(int32 Amount)
{
    if (Amount <= 0) { return; }
    CurrentHp = FMath::Max(0, CurrentHp - Amount);
    if (CurrentHp == 0)
    {
        // Knockout (not death) so quest flow can continue. Downed state handled by GameMode/character.
    }
}

bool UPISStatsComponent::ConsumeMana(int32 Amount)
{
    if (CurrentMana < Amount) { return false; }
    CurrentMana -= Amount;
    return true;
}
