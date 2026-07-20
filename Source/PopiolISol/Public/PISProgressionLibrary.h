// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PISProgressionLibrary.generated.h"

UCLASS()
class POPIOLISOL_API UPISProgressionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Progression")
	static int32 XpForLevel(int32 Level) { return 100 + Level * 75; }

	UFUNCTION(BlueprintPure, Category = "Progression")
	static int32 MaxHpForLevel(int32 Level, int32 Vitality) { return 80 + (Level - 1) * 12 + Vitality * 4; }

	UFUNCTION(BlueprintPure, Category = "Progression")
	static float SwordDamage(float Base, int32 Strength, int32 Rank) { return Base + Strength * 0.55f + Rank * 4.f; }

	UFUNCTION(BlueprintPure, Category = "Progression")
	static float BowDamage(float Base, int32 Dexterity, int32 Rank) { return Base + Dexterity * 0.60f + Rank * 3.f; }
};
