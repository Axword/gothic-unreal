// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PISSpellProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class POPIOLISOL_API APISSpellProjectile : public AActor
{
	GENERATED_BODY()

public:
	APISSpellProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spell")
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float Damage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	FString SpellId = TEXT("spell_ember");

	UFUNCTION()
	void OnImpact(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);
};
