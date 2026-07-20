// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISCharacter.h"
#include "PISStatsComponent.h"
#include "PISInventoryComponent.h"
#include "PISQuestComponent.h"
#include "PISDialogueComponent.h"
#include "PISCrimeComponent.h"
#include "PISLockedChest.h"
#include "PISSpellProjectile.h"
#include "PISMonster.h"
#include "PISWorldClock.h"
#include "PISJsonDataSubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

APISCharacter::APISCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 320.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    Stats     = CreateDefaultSubobject<UPISStatsComponent>(TEXT("Stats"));
    Inventory = CreateDefaultSubobject<UPISInventoryComponent>(TEXT("Inventory"));
    Quests    = CreateDefaultSubobject<UPISQuestComponent>(TEXT("Quests"));
    Dialogue  = CreateDefaultSubobject<UPISDialogueComponent>(TEXT("Dialogue"));
    Crime     = CreateDefaultSubobject<UPISCrimeComponent>(TEXT("Crime"));

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APISCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (auto* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (InputContext)
                {
                    Subsystem->AddMappingContext(InputContext, 0);
                }
            }
        }
    }

    // Initial starting loadout if the player has nothing.
    if (Inventory && Inventory->Items.Num() == 0)
    {
        Inventory->AddItem(TEXT("sword_01"), 1);
        Inventory->AddItem(TEXT("lockpick"), 3);
        Inventory->AddItem(TEXT("bread"), 2);
        Inventory->EquipWeapon(TEXT("sword_01"));
    }
    if (Stats)
    {
        Stats->CurrentHp = Stats->GetMaxHp();
        Stats->CurrentMana = Stats->GetMaxMana();
    }
}

void APISCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (Stamina < MaxStamina)
    {
        Stamina = FMath::Min(MaxStamina, Stamina + 8.f * DeltaSeconds);
    }
}

void APISCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (IA_Move)    EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APISCharacter::Move);
        if (IA_Look)    EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APISCharacter::Look);
        if (IA_Interact)EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &APISCharacter::OnInteract);
        if (IA_Attack)  EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &APISCharacter::OnAttack);
        if (IA_Block)
        {
            EIC->BindAction(IA_Block, ETriggerEvent::Started, this, &APISCharacter::OnBlockStart);
            EIC->BindAction(IA_Block, ETriggerEvent::Completed, this, &APISCharacter::OnBlockEnd);
        }
        if (IA_Dodge)   EIC->BindAction(IA_Dodge, ETriggerEvent::Started, this, &APISCharacter::OnDodge);
        if (IA_Cast)    EIC->BindAction(IA_Cast, ETriggerEvent::Started, this, &APISCharacter::OnCast);
        if (IA_Pause)   EIC->BindAction(IA_Pause, ETriggerEvent::Started, this, &APISCharacter::OnPause);
    }
}

void APISCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) { return; }
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Axis.IsNearlyZero()) { return; }
    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
    AddMovementInput(Forward, Axis.Y);
    AddMovementInput(Right, Axis.X);
}

void APISCharacter::Look(const FInputActionValue& Value)
{
    if (!Controller) { return; }
    const FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(-Axis.Y);
}

AActor* APISCharacter::FindBestInteractable() const
{
    UWorld* World = GetWorld();
    if (!World) { return nullptr; }
    AActor* Best = nullptr;
    float BestDistSq = 220.f * 220.f;
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* A = *It;
        if (!A) { continue; }
        // Interaction candidates: chests, characters, monsters (for skinning).
        if (A->IsA(APISLockedChest::StaticClass()) || A->IsA(ACharacter::StaticClass()) || A->IsA(APISMonster::StaticClass()))
        {
            const float D2 = FVector::DistSquared(A->GetActorLocation(), GetActorLocation());
            if (D2 < BestDistSq)
            {
                BestDistSq = D2;
                Best = A;
            }
        }
    }
    return Best;
}

void APISCharacter::OnInteract()
{
    if (Dialogue && Dialogue->IsOpen()) { return; } // dialogues are closed via specific UI button
    AActor* Target = FindBestInteractable();
    if (!Target) { return; }
    InteractWith(Target);
}

void APISCharacter::InteractWith(AActor* Target)
{
    if (!Target) { return; }
    if (APISLockedChest* Chest = Cast<APISLockedChest>(Target))
    {
        if (Chest->bOpened) { return; }
        if (Inventory && Inventory->CountItem(TEXT("key_watch")) > 0)
        {
            Chest->OpenWithKey(TEXT("key_watch"));
            return;
        }
        Chest->BeginLockpick(Stats ? Stats->Skills.Lockpick : 1);
        return;
    }
    if (ACharacter* NPC = Cast<ACharacter>(Target))
    {
        // The NPC controller (or our own component if it has one) is responsible for starting the dialogue.
        FString DialogueId;
        if (Target->ActorHasTag(TEXT("PIS_NPC")))
        {
            // Default dialogue id from tag "PIS_Dialogue:<id>" or fall back to ID from npc id.
            for (const FName& Tag : Target->Tags)
            {
                const FString TS = Tag.ToString();
                if (TS.StartsWith(TEXT("PIS_Dialogue:")))
                {
                    DialogueId = TS.RightChop(13);
                    break;
                }
            }
        }
        if (DialogueId.IsEmpty())
        {
            // Fall back to actor name -> dialogue_<name>_intro.
            DialogueId = FString::Printf(TEXT("dialogue_%s_intro"), *Target->GetName().ToLower());
        }
        if (Dialogue)
        {
            Dialogue->OpenDialogue(DialogueId);
        }
        return;
    }
    if (APISMonster* Monster = Cast<APISMonster>(Target))
    {
        if (Monster->IsDowned())
        {
            SkinMonster(Monster);
        }
    }
}

void APISCharacter::OnAttack()
{
    if (CombatMode == EPISCombatMode::Bow)
    {
        if (bIsDrawingBow) { ReleaseBowShot(); }
        else { BeginBowDraw(); }
        return;
    }
    if (CombatMode == EPISCombatMode::Magic)
    {
        CastActiveSpell();
        return;
    }
    PerformSwordAttack();
}

void APISCharacter::PerformSwordAttack()
{
    if (bIsAttacking) { return; }
    if (GetWorld()->GetTimeSeconds() < NextAttackAllowedAt) { return; }
    bIsAttacking = true;
    NextAttackAllowedAt = GetWorld()->GetTimeSeconds() + AttackCooldown;

    const float Damage = UPISProgressionLibrary::SwordDamage(
        SwordDamageBase,
        Stats ? Stats->Primary.Strength : 0,
        Stats ? Stats->Skills.Sword : 0);

    FHitResult Hit;
    FCollisionQueryParams P(SCENE_QUERY_STAT(Melee), false, this);
    const FVector Start = GetActorLocation() + GetActorForwardVector() * 60.f;
    const FVector End   = Start + GetActorForwardVector() * 180.f;
    FVector BoxHalf(40.f, 40.f, 40.f);
    TArray<FHitResult> Hits;
    GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeBox(BoxHalf), P);
    for (const FHitResult& H : Hits)
    {
        AActor* Other = H.GetActor();
        if (!Other || Other == this) { continue; }
        if (APISMonster* M = Cast<APISMonster>(Other))
        {
            M->ReceiveDamage(Damage, GetActorLocation(), this);
        }
        else if (ACharacter* C = Cast<ACharacter>(Other))
        {
            // Friendly fire suppression: only damage if tagged hostile.
            if (C->ActorHasTag(TEXT("PIS_Hostile")))
            {
                FDamageEvent DE;
                C->TakeDamage(Damage * 0.6f, DE, GetController(), this);
            }
        }
    }
    GetWorldTimerManager().SetTimerForNextTick([this]() { bIsAttacking = false; });
}

void APISCharacter::BeginBowDraw()
{
    if (bIsDrawingBow || bIsAttacking) { return; }
    bIsDrawingBow = true;
    BowDrawStartedAt = GetWorld()->GetTimeSeconds();
}

void APISCharacter::ReleaseBowShot()
{
    if (!bIsDrawingBow) { return; }
    bIsDrawingBow = false;
    const float Drawn = GetWorld()->GetTimeSeconds() - BowDrawStartedAt;
    if (Drawn < 0.2f) { return; } // tap = no shot
    const float Damage = UPISProgressionLibrary::BowDamage(
        BowDamageBase,
        Stats ? Stats->Primary.Dexterity : 0,
        Stats ? Stats->Skills.Bow : 0);

    FHitResult Hit;
    FCollisionQueryParams P(SCENE_QUERY_STAT(Bow), false, this);
    const FVector Start = Camera ? Camera->GetComponentLocation() : GetActorLocation();
    const FVector End   = Start + (Camera ? Camera->GetForwardVector() : GetActorForwardVector()) * 8000.f;
    FHitResult HR;
    if (GetWorld()->LineTraceSingleByChannel(HR, Start, End, ECC_Pawn, P))
    {
        if (APISMonster* M = Cast<APISMonster>(HR.GetActor()))
        {
            M->ReceiveDamage(Damage, Start, this);
        }
    }
}

void APISCharacter::OnBlockStart() { bIsBlocking = true; }
void APISCharacter::OnBlockEnd()   { bIsBlocking = false; }
void APISCharacter::OnDodge()
{
    if (Stamina < 25.f) { return; }
    Stamina -= 25.f;
    LaunchCharacter(GetActorForwardVector() * 600.f + FVector(0, 0, 60.f), true, false);
}

void APISCharacter::OnCast()
{
    CastActiveSpell();
}

void APISCharacter::OnPause()
{
    // Delegate to the game instance / UI subsystem. Hook in BP.
}

void APISCharacter::CastActiveSpell()
{
    if (!Stats) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    if (!Data) { return; }
    const FString SpellId = ActiveSpell == EPISSpell::Ember ? TEXT("spell_ember") : TEXT("spell_frost");
    const TSharedPtr<FJsonObject>* SpellObj = Data->Find(SpellId);
    if (!SpellObj || !SpellObj->IsValid()) { return; }
    int32 ManaCost = 15;
    (*SpellObj)->TryGetNumberField(TEXT("mana"), ManaCost);
    if (!Stats->ConsumeMana(ManaCost)) { return; }
    TSubclassOf<APISSpellProjectile> Cls = (ActiveSpell == EPISSpell::Ember) ? EmberProjectileClass : FrostProjectileClass;
    if (!Cls) { return; }
    FActorSpawnParameters SP;
    SP.Owner = this;
    SP.Instigator = this;
    const FVector SpawnLoc = (Camera ? Camera->GetComponentLocation() : GetActorLocation()) + (Camera ? Camera->GetForwardVector() : GetActorForwardVector()) * 60.f;
    const FRotator SpawnRot = Camera ? Camera->GetComponentRotation() : GetActorRotation();
    if (APISSpellProjectile* P = GetWorld()->SpawnActor<APISSpellProjectile>(Cls, SpawnLoc, SpawnRot, SP))
    {
        P->Damage = (ActiveSpell == EPISSpell::Ember) ? SpellDamageEmber : SpellDamageFrost;
        P->Damage *= (1.f + 0.05f * (ActiveSpell == EPISSpell::Ember ? Stats->Skills.FireMagic : Stats->Skills.IceMagic));
    }
}

bool APISCharacter::SkinMonster(APISMonster* Monster)
{
    if (!Monster || !Stats) { return false; }
    if (Stats->Skills.Skinning <= 0) { return false; }
    SkinningLootCount += 1;
    if (Inventory)
    {
        // Convert the monster's id to a trophy id (best-effort, uses JSON catalog).
        UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
        if (UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr)
        {
            const TSharedPtr<FJsonObject>* Mobj = Data->Find(Monster->MonsterId);
            if (Mobj && Mobj->IsValid())
            {
                FString Trophy;
                if ((*Mobj)->TryGetStringField(TEXT("trophy_item_id"), Trophy) && !Trophy.IsEmpty())
                {
                    Inventory->AddItem(Trophy, 1);
                }
            }
        }
    }
    return true;
}
