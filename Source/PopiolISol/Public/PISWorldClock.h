#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PISWorldClock.generated.h"
UCLASS(Blueprintable) class POPIOLISOL_API APISWorldClock:public AActor { GENERATED_BODY() public:
 APISWorldClock(); UPROPERTY(EditAnywhere,BlueprintReadWrite) float Hour=7; UPROPERTY(EditAnywhere) float GameMinutesPerRealSecond=4;
 UPROPERTY(BlueprintReadOnly) int32 Day=1; UFUNCTION(BlueprintPure) bool IsNight()const{return Hour>=20||Hour<6;} UFUNCTION(BlueprintCallable) void SleepTo(float TargetHour);
 virtual void Tick(float DeltaSeconds)override;
};
