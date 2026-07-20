// Copyright Epic Games, Inc. All Rights Reserved.
#include "PISInventoryComponent.h"

bool UPISInventoryComponent::AddItem(const FString& ItemId, int32 Count)
{
	if (ItemId.IsEmpty() || Count <= 0)
	{
		return false;
	}

	for (FPISInventoryStack& Stack : Items)
	{
		if (Stack.ItemId == ItemId)
		{
			Stack.Count += Count;
			return true;
		}
	}

	FPISInventoryStack NewStack;
	NewStack.ItemId = ItemId;
	NewStack.Count = Count;
	Items.Add(NewStack);
	return true;
}

bool UPISInventoryComponent::RemoveItem(const FString& ItemId, int32 Count)
{
	if (Count <= 0)
	{
		return false;
	}

	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		if (Items[Index].ItemId != ItemId)
		{
			continue;
		}

		if (Items[Index].Count < Count)
		{
			return false;
		}

		Items[Index].Count -= Count;
		if (Items[Index].Count == 0)
		{
			Items.RemoveAt(Index);
		}

		if (EquippedWeaponId == ItemId && CountItem(ItemId) == 0)
		{
			EquippedWeaponId.Empty();
		}
		return true;
	}

	return false;
}

int32 UPISInventoryComponent::CountItem(const FString& ItemId) const
{
	for (const FPISInventoryStack& Stack : Items)
	{
		if (Stack.ItemId == ItemId)
		{
			return Stack.Count;
		}
	}
	return 0;
}

bool UPISInventoryComponent::EquipWeapon(const FString& ItemId)
{
	if (CountItem(ItemId) < 1)
	{
		return false;
	}
	EquippedWeaponId = ItemId;
	return true;
}
