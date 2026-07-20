// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PISCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UPISStatsComponent;
class UPISInventoryComponent;
class UPISQuestComponent;
class UPISDialogueComponent;
class UPISCrimeComponent;
class APISSpellProjectile;
class APISMonster;
class APISLockedChest;

UENUM(BlueprintType)
enum class EPISCombatMode : uint8 { Unarmed, Sword, Bow, Magic };

UENUM(BlueprintType)
enum class EPISSpell : uint8 { Ember, Frost };

UCLASS()
class POPIOLISOL_API APISCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APISCharacter();

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PIS") TObjectPtr<USpringArmComponent> SpringArm;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PIS") TObjectPtr<UCameraComponent> Camera;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PIS") TObjectPtr<UPISStatsComponent> Stats;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PIS") TObjectPtr<UPISInventoryComponent> Inventory;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PIS") TObjectPtr<UPISQuestComponent> Quests;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PIS") TObjectPtr<UPISDialogueComponent> Dialogue;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PIS") TObjectPtr<UPISCrimeComponent> Crime;

    // Enhanced Input assets - assigned in BP_PISCharacter or by Python script.
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputMappingContext> InputContext;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Move;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Look;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Interact;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Attack;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Block;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Dodge;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Cast;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Pause;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Inventory;
    UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> IA_Journal;

    UPROPERTY(EditDefaultsOnly, Category = "Combat") TSubclassOf<APISSpellProjectile> EmberProjectileClass;
    UPROPERTY(EditDefaultsOnly, Category = "Combat") TSubclassOf<APISSpellProjectile> FrostProjectileClass;

    UPROPERTY(BlueprintReadOnly, Category = "Combat") EPISCombatMode CombatMode = EPISCombatMode::Sword;
    UPROPERTY(BlueprintReadOnly, Category = "Combat") EPISSpell ActiveSpell = EPISSpell::Ember;

    /** Stamina for block/dodge if enabled (kept minimal for clarity). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float Stamina = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float MaxStamina = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float AttackCooldown = 0.6f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float BowDrawTime = 0.8f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float BowDamageBase = 18.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float SwordDamageBase = 14.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float SpellDamageEmber = 22.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float SpellDamageFrost = 18.f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat") bool bIsBlocking = false;
    UPROPERTY(BlueprintReadOnly, Category = "Combat") bool bIsDrawingBow = false;
    UPROPERTY(BlueprintReadOnly, Category = "Combat") bool bIsAttacking = false;
    UPROPERTY(BlueprintReadOnly, Category = "Combat") float NextAttackAllowedAt = 0.f;
    UPROPERTY(BlueprintReadOnly, Category = "Combat") float BowDrawStartedAt = 0.f;

    // Skinning
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skinning") int32 SkinningLootCount = 0;

    // Input handlers (called by BP or Python-assigned bindings).
    UFUNCTION(BlueprintCallable, Category = "Input") void Move(const FInputActionValue& Value);
    UFUNCTION(BlueprintCallable, Category = "Input") void Look(const FInputActionValue& Value);
    UFUNCTION(BlueprintCallable, Category = "Input") void OnInteract();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnAttack();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnBlockStart();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnBlockEnd();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnDodge();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnCast();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnPause();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnInventory();
    UFUNCTION(BlueprintCallable, Category = "Input") void OnJournal();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCombatMode(EPISCombatMode NewMode) { CombatMode = NewMode; }
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetActiveSpell(EPISSpell NewSpell) { ActiveSpell = NewSpell; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformSwordAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void BeginBowDraw();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ReleaseBowShot();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void CastActiveSpell();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    AActor* FindBestInteractable() const;

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void InteractWith(AActor* Target);

    /** Skin a downed monster and grant trophies if skinning skill is sufficient. */
    UFUNCTION(BlueprintCallable, Category = "Skinning")
    bool SkinMonster(APISMonster* Monster);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
