// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISSpellProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APISSpellProjectile::APISSpellProjectile()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->InitSphereRadius(12.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Collision->OnComponentHit.AddDynamic(this, &APISSpellProjectile::OnImpact);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 1500.f;
	Movement->MaxSpeed = 1500.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 5.f;
}

void APISSpellProjectile::OnImpact(
	UPrimitiveComponent* /*HitComponent*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	FVector /*NormalImpulse*/,
	const FHitResult& Hit)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		const FVector ShotDir = GetVelocity().GetSafeNormal();
		UGameplayStatics::ApplyPointDamage(
			OtherActor,
			Damage,
			ShotDir,
			Hit,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass());
	}
	Destroy();
}
