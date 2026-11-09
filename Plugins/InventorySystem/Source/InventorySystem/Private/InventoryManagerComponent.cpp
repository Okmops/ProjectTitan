// Copyright Epic Games, Inc. All Rights Reserved.


#include "InventoryManagerComponent.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Inventory_Item, "Inventory.Item");

UInventoryManagerComponent::UInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const TMap<FGameplayTag, int32>& UInventoryManagerComponent::GetItemStacks() const
{
	return ItemStacks;
}

void UInventoryManagerComponent::UpdateItemStacks(const TMap<FGameplayTag, int32>& Items)
{
	// copy the container
	// NOTE: this will not call the count update delegates
	ItemStacks = Items;
}

bool UInventoryManagerComponent::AddItem(FGameplayTag ItemTag, int32 Count /*= 1*/)
{
	// ensure this is an item tag and the count is valid
	if (IsItemTag(ItemTag) && Count > 0)
	{
		// get the item count before the change
		const int32 PreviousCount = GetItemCount(ItemTag);

		// ensure we don't overflow the item count
		int32 UpdatedCount =  0;
		if (!FMath::AddAndCheckForOverflow(PreviousCount, Count, UpdatedCount))
		{
			UpdatedCount = INT_MAX;
		}

		// add or update the count for the tag
		ItemStacks.Add(ItemTag, UpdatedCount);

		// has the count changed?
		if (PreviousCount != UpdatedCount)
		{
			// calculate the actual count delta applied
			const int32 CountDelta = UpdatedCount - PreviousCount;

			// call the delegate
			OnItemCountChanged.Broadcast(ItemTag, CountDelta, UpdatedCount);
		}

		return true;
	}

	return false;
}

bool UInventoryManagerComponent::RemoveItem(FGameplayTag ItemTag, int32 Count /*= 1*/)
{
	// ensure this is an item tag and the count is valid
	if (IsItemTag(ItemTag) && Count > 0)
	{
		// get the item count before the change
		const int32 PreviousCount = GetItemCount(ItemTag);

		// ensure we don't go below zero items
		int32 UpdatedCount = 0;

		// ensure we don't underflow the item count
		if (!FMath::SubtractAndCheckForOverflow(PreviousCount, Count, UpdatedCount))
		{
			// set to zero on underflow
			UpdatedCount = 0;
		}
		else
		{
			// clamp to zero
			UpdatedCount = FMath::Max(UpdatedCount, 0);
		}

		// do we still have items?
		if (UpdatedCount > 0)
		{
			// update the count for the tag
			ItemStacks.Add(ItemTag, UpdatedCount);
		}
		else
		{
			// remove the tag
			ItemStacks.Remove(ItemTag);
		}
				
		// has the count changed?
		if (PreviousCount != UpdatedCount)
		{
			// calculate the actual count delta applied
			const int32 CountDelta = UpdatedCount - PreviousCount;

			// call the delegate
			OnItemCountChanged.Broadcast(ItemTag, CountDelta, UpdatedCount);

			return true;
		}
	}

	return false;
}

bool UInventoryManagerComponent::HasItem(FGameplayTag ItemTag) const
{
	// ensure this is an item tag
	if (IsItemTag(ItemTag))
	{
		// we have the item if the tag count is greater than zero
		return GetItemCount(ItemTag) > 0;
	}
	
	return false;
}

int32 UInventoryManagerComponent::GetItemCount(FGameplayTag ItemTag) const
{
	// ensure this is an item tag
	if (IsItemTag(ItemTag))
	{
		// try to find the item tag count in the container
		if (const int32* Count = ItemStacks.Find(ItemTag))
		{
			return *Count;
		}
	}

	// if we do not have the tag or the tag is invalid, return zero
	return 0;
}

void UInventoryManagerComponent::GetAllChildItemTags(FGameplayTag ParentTag, TArray<FGameplayTag>& OutTags) const
{
	// ensure the return array is empty
	OutTags.Empty();

	// we only care about item tags
	if (IsItemTag(ParentTag))
	{
		// iterate through the container
		for (const auto& Current : ItemStacks)
		{
			// do we have a match?
			if (Current.Key.MatchesTag(ParentTag))
			{
				// add the tag to the output
				OutTags.Add(Current.Key);
			}
		}
	}
}

bool UInventoryManagerComponent::IsItemTag(const FGameplayTag& ItemTag) const
{
	// ensure the tag's parent is the inventory item root
	return ItemTag.MatchesTag(TAG_Inventory_Item);
}