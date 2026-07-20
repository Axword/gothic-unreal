// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISWorldClock.h"

APISWorldClock::APISWorldClock()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APISWorldClock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Hour += DeltaSeconds * GameMinutesPerRealSecond / 60.f;
	while (Hour >= 24.f)
	{
		Hour -= 24.f;
		++Day;
	}
}

void APISWorldClock::SleepTo(float TargetHour)
{
	Hour = FMath::Clamp(TargetHour, 0.f, 23.99f);
}
