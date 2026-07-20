// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PISNPC.generated.h"

class APISNPCController;

UCLASS()
class POPIOLISOL_API APISNPC : public ACharacter
{
    GENERATED_BODY()

public:
    APISNPC();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString NpcId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString Faction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString Role;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString ScheduleId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString DialogueId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString Attitude;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString CrimeReaction;

    /** Marker to start at; will be resolved against world location JSON. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString SpawnMarker;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") bool bIsInvulnerable = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") int32 Level = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") FString SkillToTeach;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PIS") int32 TeachingCost = 50;

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void ApplyFromJson();

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void MoveToMarker(const FString& Marker);

    UFUNCTION(BlueprintCallable, Category = "PIS")
    void Talk();

    virtual void BeginPlay() override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(BlueprintReadOnly, Category = "PIS") TObjectPtr<APISNPCController> NpcController;
};
