// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PISMonster.generated.h"

class APISCharacter;
class APISMonsterController;

UENUM(BlueprintType)
enum class EPISMonsterState : uint8 { Idle, Patrolling, Chasing, Attacking, Returning, Downed, Dead };

UCLASS()
class POPIOLISOL_API APISMonster : public ACharacter
{
    GENERATED_BODY()

public:
    APISMonster();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString MonsterId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") int32 Level = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") int32 MaxHp = 50;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") int32 CurrentHp = 50;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") float Armor = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") float AttackDamage = 10.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") float AttackRange = 160.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") float SightRadius = 1400.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") float LoseRadius = 2200.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") float MoveSpeed = 350.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString FactionId = TEXT("wild");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") bool bAggressiveAtNight = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") bool bPack = false;

    UPROPERTY(BlueprintReadOnly, Category = "PIS") EPISMonsterState State = EPISMonsterState::Idle;
    UPROPERTY(BlueprintReadOnly, Category = "PIS") bool bDowned = false;

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void ReceiveDamage(float Damage, FVector HitFrom, AActor* Instigator);

    UFUNCTION(BlueprintCallable, Category = "PIS")
    bool IsDowned() const { return bDowned; }

    UFUNCTION(BlueprintCallable, Category = "PIS")
    bool IsDead() const { return State == EPISMonsterState::Dead; }

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void SetState(EPISMonsterState NewState);

    /** Drop loot to the ground (player picks up via interaction). */
    UFUNCTION(BlueprintCallable, Category = "PIS")
    void DropLoot();

    virtual void BeginPlay() override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    /** Patrol origin used by AI controller on return. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FVector PatrolOrigin;
    /** Pack members (only set on bPack monsters) */
    UPROPERTY(BlueprintReadOnly, Category = "PIS") TArray<TWeakObjectPtr<APISMonster>> Pack;
};
