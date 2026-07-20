// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISNPC.h"
#include "PISNPCController.h"
#include "PISCharacter.h"
#include "PISDialogueComponent.h"
#include "PISJsonDataSubsystem.h"
#include "PISWorldClock.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"

APISNPC::APISNPC()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = APISNPCController::StaticClass();
    GetCharacterMovement()->MaxWalkSpeed = 250.f;
    GetCharacterMovement()->bUseRVOAvoidance = true;
}

void APISNPC::BeginPlay()
{
    Super::BeginPlay();
    ApplyFromJson();
    // Initial marker placement.
    if (!SpawnMarker.IsEmpty())
    {
        MoveToMarker(SpawnMarker);
    }
}

void APISNPC::ApplyFromJson()
{
    UWorld* World = GetWorld();
    if (!World) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    if (!Data) { return; }
    const TSharedPtr<FJsonObject>* Obj = Data->Find(NpcId);
    if (!Obj || !Obj->IsValid()) { return; }
    FString Tmp;
    if ((*Obj)->TryGetStringField(TEXT("name"), Tmp))        { /* set display name */ }
    if ((*Obj)->TryGetStringField(TEXT("faction"), Tmp))     { Faction = Tmp; }
    if ((*Obj)->TryGetStringField(TEXT("role"), Tmp))        { Role = Tmp; }
    if ((*Obj)->TryGetStringField(TEXT("schedule_id"), Tmp)) { ScheduleId = Tmp; }
    if ((*Obj)->TryGetStringField(TEXT("attitude"), Tmp))    { Attitude = Tmp; }
    if ((*Obj)->TryGetStringField(TEXT("crime_reaction"), Tmp)) { CrimeReaction = Tmp; }
    int32 Lv = 1; if ((*Obj)->TryGetNumberField(TEXT("level"), Lv)) { Level = Lv; }
}

void APISNPC::MoveToMarker(const FString& Marker)
{
    UWorld* World = GetWorld();
    if (!World) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    if (!Data) { return; }
    FVector Loc = GetActorLocation();
    for (const TSharedPtr<FJsonValue>& V : Data->GetAllRecords())
    {
        const TSharedPtr<FJsonObject> O = V.IsValid() ? V->AsObject() : nullptr;
        if (!O.IsValid()) { continue; }
        if (O->GetStringField(TEXT("id")) == FString::Printf(TEXT("marker_%s"), *Marker))
        {
            double X = 0, Y = 0, Z = 0;
            O->TryGetNumberField(TEXT("x"), X);
            O->TryGetNumberField(TEXT("y"), Y);
            O->TryGetNumberField(TEXT("z"), Z);
            Loc = FVector(X, Y, Z);
            break;
        }
    }
    UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(World);
    if (Nav)
    {
        APISNPCController* Ctrl = Cast<APISNPCController>(GetController());
        if (Ctrl) { Nav->SimpleMoveToLocation(Ctrl, Loc); }
    }
    else
    {
        // Fallback: teleport out of sight of the player.
        APISCharacter* Player = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
        if (Player)
        {
            const float D = FVector::Dist(Player->GetActorLocation(), Loc);
            if (D < 1500.f)
            {
                // Move away from the player
                const FVector Away = (Loc - Player->GetActorLocation()).GetSafeNormal2D() * 1500.f;
                Loc = Player->GetActorLocation() + Away + FVector(0, 0, 100.f);
            }
        }
        SetActorLocation(Loc, false, nullptr, ETeleportType::TeleportPhysics);
    }
}

void APISNPC::Talk()
{
    UWorld* World = GetWorld();
    if (!World) { return; }
    APISCharacter* Player = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
    if (!Player) { return; }
    UPISDialogueComponent* Dlg = Player->FindComponentByClass<UPISDialogueComponent>();
    if (Dlg)
    {
        FString DlgId = DialogueId;
        if (DlgId.IsEmpty()) { DlgId = FString::Printf(TEXT("dialogue_%s_intro"), *NpcId); }
        Dlg->OpenDialogue(DlgId);
    }
}

float APISNPC::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsInvulnerable) { return 0.f; }
    if (DamageAmount > 0.f) { SetActorEnableCollision(false); }
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
