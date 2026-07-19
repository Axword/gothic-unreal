#include "PISSpellProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
APISSpellProjectile::APISSpellProjectile(){Collision=CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));RootComponent=Collision;Collision->InitSphereRadius(12);Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));Collision->OnComponentHit.AddDynamic(this,&APISSpellProjectile::OnImpact);Movement=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));Movement->InitialSpeed=1500;Movement->MaxSpeed=1500;InitialLifeSpan=5;}
void APISSpellProjectile::OnImpact(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,FVector,int32,bool,const FHitResult&){if(Other&&Other!=GetOwner())UGameplayStatics::ApplyPointDamage(Other,Damage,GetVelocity().GetSafeNormal(),FHitResult(),GetInstigatorController(),this,nullptr);Destroy();}
