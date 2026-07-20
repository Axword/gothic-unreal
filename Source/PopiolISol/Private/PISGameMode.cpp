// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISGameMode.h"
#include "PISCharacter.h"
#include "PISHUD.h"
#include "PISMainMenuWidget.h"
#include "PISStatsComponent.h"
#include "PISJsonDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Blueprint/UserWidget.h"
#include "Dom/JsonObject.h"

APISGameMode::APISGameMode()
{
    DefaultPawnClass = APISCharacter::StaticClass();
    HUDClass = APISHUD::StaticClass();
}

void APISGameMode::BeginPlay()
{
    Super::BeginPlay();
    // On MainMenu map there is no player pawn; spawn a default controller and show the menu.
    UWorld* World = GetWorld();
    if (!World) { return; }
    const FString MapName = World->GetMapName();
    if (MapName.Contains(TEXT("MainMenu")))
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        {
            UPISMainMenuWidget* Menu = CreateWidget<UPISMainMenuWidget>(PC, UPISMainMenuWidget::StaticClass());
            if (Menu) { Menu->AddToViewport(50); }
        }
    }
}

void APISGameMode::RespawnPlayer()
{
    APISCharacter* Player = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Player) { return; }
    UWorld* World = GetWorld();
    if (!World) { return; }
    UGameInstance* GI = UGameplayStatics::GetGameInstance(World);
    UPISJsonDataSubsystem* Data = GI ? GI->GetSubsystem<UPISJsonDataSubsystem>() : nullptr;
    FVector SpawnLoc(0, 0, 200);
    if (Data)
    {
        const TSharedPtr<FJsonObject>* O = Data->Find(TEXT("marker_spawn_arrival"));
        if (O && O->IsValid())
        {
            double X=0,Y=0,Z=0; (*O)->TryGetNumberField(TEXT("x"),X); (*O)->TryGetNumberField(TEXT("y"),Y); (*O)->TryGetNumberField(TEXT("z"),Z);
            SpawnLoc = FVector(X,Y,Z);
        }
    }
    Player->SetActorLocation(SpawnLoc, false, nullptr, ETeleportType::TeleportPhysics);
    if (UPISStatsComponent* Stats = Player->FindComponentByClass<UPISStatsComponent>())
    {
        Stats->CurrentHp = Stats->GetMaxHp();
        Stats->CurrentMana = Stats->GetMaxMana();
    }
}

void APISGameMode::KnockoutPlayer()
{
    APISCharacter* Player = Cast<APISCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Player) { return; }
    RespawnPlayer();
}
