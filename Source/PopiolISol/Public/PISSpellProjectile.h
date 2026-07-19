#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PISSpellProjectile.generated.h"
class USphereComponent; class UProjectileMovementComponent;
UCLASS() class POPIOLISOL_API APISSpellProjectile:public AActor { GENERATED_BODY() public: APISSpellProjectile(); UPROPERTY(VisibleAnywhere) USphereComponent* Collision; UPROPERTY(VisibleAnywhere) UProjectileMovementComponent* Movement; UPROPERTY(EditAnywhere,BlueprintReadWrite) float Damage=25; UPROPERTY(EditAnywhere,BlueprintReadWrite) FString SpellId=TEXT("spell_ember"); UFUNCTION() void OnImpact(UPrimitiveComponent*,AActor*,UPrimitiveComponent*,FVector,int32,bool,const FHitResult&);};
