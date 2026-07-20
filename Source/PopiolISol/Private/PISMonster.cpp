// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISMonster.h"
#include "PISCharacter.h"
#include "PISMonsterController.h"
#include "PISJsonDataSubsystem.h"
#include "PISInventoryComponent.h"
#include "PISStatsComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"

APISMonster::APISMonster()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = APISMonsterController::StaticClass();

    GetCharacterMovement()->MaxWalkSpeed = 350.f;
    GetCharacterMovement()->bUseRVOAvoidance = true;
}

void APISMonster::BeginPlay()
{
    Super::BeginPlay();
    if (MonsterId.Len() > 0)
    {
        UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
        if (UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr)
        {
            const TSharedPtr<FJsonObject>* Obj = Data->Find(MonsterId);
            if (Obj && Obj->IsValid())
            {
                int32 Lv = 1; if ((*Obj)->TryGetNumberField(TEXT("level"), Lv)) { Level = Lv; }
                MaxHp = FMath::Max(1, 20 + Level * 12);
                CurrentHp = MaxHp;
                double Dmg = 0; if ((*Obj)->TryGetNumberField(TEXT("damage"), Dmg)) { AttackDamage = static_cast<float>(Dmg); }
                double Hp = 0;  if ((*Obj)->TryGetNumberField(TEXT("hp"), Hp))     { MaxHp = static_cast<int32>(Hp); CurrentHp = MaxHp; }
                FString Faction; if ((*Obj)->TryGetStringField(TEXT("faction"), Faction)) { FactionId = Faction; }
                bool bPack = false; if ((*Obj)->TryGetBoolField(TEXT("pack"), bPack)) { this->bPack = bPack; }
                bool bNight = true; if ((*Obj)->TryGetBoolField(TEXT("aggressive_at_night"), bNight)) { bAggressiveAtNight = bNight; }
            }
        }
    }
    PatrolOrigin = GetActorLocation();
}

void APISMonster::SetState(EPISMonsterState NewState)
{
    State = NewState;
    if (NewState == EPISMonsterState::Dead)
    {
        GetCharacterMovement()->DisableMovement();
        SetActorEnableCollision(false);
    }
}

void APISMonster::ReceiveDamage(float Damage, FVector HitFrom, AActor* Instigator)
{
    if (State == EPISMonsterState::Dead) { return; }
    const float Net = FMath::Max(0.f, Damage - Armor);
    CurrentHp = FMath::Max(0, CurrentHp - FMath::RoundToInt(Net));
    if (CurrentHp == 0)
    {
        if (bDowned)
        {
            SetState(EPISMonsterState::Dead);
            DropLoot();
        }
        else
        {
            // First lethal hit -> downed (knockout), not death, so the player can skin.
            bDowned = true;
            SetState(EPISMonsterState::Downed);
            GetCharacterMovement()->DisableMovement();
        }
    }
    else if (APISMonsterController* AI = Cast<APISMonsterController>(GetController()))
    {
        AI->OnHitReact(HitFrom, Instigator);
    }
}

float APISMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    ReceiveDamage(DamageAmount, DamageCauser ? DamageCauser->GetActorLocation() : GetActorLocation(), DamageCauser);
    return DamageAmount;
}

void APISMonster::DropLoot()
{
    UWorld* World = GetWorld();
    if (!World) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    if (!Data) { return; }

    // Find loot table by spawn id "loot_<monster_id>".
    const FString LootId = FString::Printf(TEXT("loot_%s"), *MonsterId);
    const TSharedPtr<FJsonObject>* Loot = Data->Find(LootId);
    if (!Loot || !Loot->IsValid()) { return; }
    const TArray<TSharedPtr<FJsonValue>>* Entries = nullptr;
    if (!(*Loot)->TryGetArrayField(TEXT("entries"), Entries) || !Entries) { return; }

    APISCharacter* Player = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
    UPISInventoryComponent* Inv = Player ? Player->FindComponentByClass<UPISInventoryComponent>() : nullptr;
    if (Inv)
    {
        for (const TSharedPtr<FJsonValue>& V : *Entries)
        {
            const TSharedPtr<FJsonObject> E = V.IsValid() ? V->AsObject() : nullptr;
            if (!E.IsValid()) { continue; }
            FString Iid; if (E->TryGetStringField(TEXT("item_id"), Iid))
            {
                int32 Count = 1; E->TryGetNumberField(TEXT("count"), Count);
                Inv->AddItem(Iid, Count);
            }
            else
            {
                // legacy "item" field
                FString Item; if (E->TryGetStringField(TEXT("item"), Item))
                {
                    int32 Count = 1; E->TryGetNumberField(TEXT("count"), Count);
                    Inv->AddItem(Item, Count);
                }
            }
        }
    }
    if (Player)
    {
        if (UPISStatsComponent* Stats = Player->FindComponentByClass<UPISStatsComponent>())
        {
            Stats->AwardXp(20 + Level * 5);
        }
    }
}
