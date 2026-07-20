// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISLockedChest.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PISInventoryComponent.h"

APISLockedChest::APISLockedChest()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetBoxExtent(FVector(80.f));
	Trigger->SetCollisionProfileName(TEXT("Trigger"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	Mesh->SetupAttachment(RootComponent);
}

bool APISLockedChest::BeginLockpick(int32 Rank)
{
	if (bOpened || Rank < RequiredLockRank)
	{
		return false;
	}
	Cursor = 0;
	bPicking = true;
	return true;
}

bool APISLockedChest::SubmitLockTurn(bool bRight, UPISInventoryComponent* Inventory)
{
	if (!bPicking || bOpened || !Inventory || Sequence.IsEmpty())
	{
		return false;
	}

	const TCHAR Expected = Sequence[Cursor];
	const bool bMatch = (bRight && Expected == TEXT('R')) || (!bRight && Expected == TEXT('L'));
	if (bMatch)
	{
		++Cursor;
		if (Cursor >= Sequence.Len())
		{
			bOpened = true;
			bPicking = false;
			return true;
		}
		// Correct turn but sequence not finished yet.
		return false;
	}

	// Wrong turn: consume a lockpick and reset the sequence.
	Inventory->RemoveItem(TEXT("lockpick"), 1);
	Cursor = 0;
	return false;
}

bool APISLockedChest::OpenWithKey(const FString& KeyId)
{
	if (bOpened || KeyId != TEXT("key_watch"))
	{
		return false;
	}
	bOpened = true;
	bPicking = false;
	return true;
}

FString APISLockedChest::Prompt() const
{
	if (bOpened)
	{
		return TEXT("Przeszukaj skrzynię");
	}
	return FString::Printf(TEXT("Zamek %d: %s"), RequiredLockRank, *ChestId);
}
