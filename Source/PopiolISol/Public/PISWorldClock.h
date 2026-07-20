// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PISWorldClock.generated.h"

UCLASS(Blueprintable)
class POPIOLISOL_API APISWorldClock : public AActor
{
	GENERATED_BODY()

public:
	APISWorldClock();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float Hour = 7.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float GameMinutesPerRealSecond = 4.f;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	int32 Day = 1;

	UFUNCTION(BlueprintPure, Category = "Time")
	bool IsNight() const { return Hour >= 20.f || Hour < 6.f; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	void SleepTo(float TargetHour);

	virtual void Tick(float DeltaSeconds) override;
};
