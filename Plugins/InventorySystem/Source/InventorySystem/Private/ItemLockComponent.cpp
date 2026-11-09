// Copyright Epic Games, Inc. All Rights Reserved.


#include "ItemLockComponent.h"
#include "InventoryManagerComponent.h"
#include "InventoryInterface.h"

UItemLockComponent::UItemLockComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UItemLockComponent::TryUnlock(AActor* UnlockingActor)
{
	// fail if we're already unlocked
	if (bUnlocked)
	{
		return false;
	}

	// validate the actor
	if (IsValid(UnlockingActor))
	{
		// get the inventory manager comp
		UInventoryManagerComponent* Inventory = nullptr;

		// first try to get it through the inventory interface
		if (IInventoryInterface* InventoryInterface = Cast<IInventoryInterface>(UnlockingActor))
		{
			Inventory = InventoryInterface->GetInventoryManagerComponent();
		}

		// if we fail, search the actor for an inventory manager comp
		if (!Inventory)
		{
			// get the inventory manager comp
			Inventory = UnlockingActor->FindComponentByClass<UInventoryManagerComponent>();
		}

		// get the actor's inventory manager
		if (Inventory)
		{
			// test each key condition. We only unlock if we satisfy all of them.
			for (const FKeyItemDescription& Current : KeyItems)
			{
				// check how many items the actor holds
				int32 Held = Inventory->GetItemCount(Current.ItemTag);

				// can we satisfy this key item condition?
				if (Held < Current.Amount)
				{
					return false;
				}
			}

			// we can unlock. Should we consume the keys?
			if (bConsumeItemsOnUnlock)
			{
				for (const FKeyItemDescription& Current : KeyItems)
				{
					Inventory->RemoveItem(Current.ItemTag, Current.Amount);
				}
			}

			// unlock the component and call the delegate
			bUnlocked = true;

			OnUnlocked.Broadcast(UnlockingActor);

			return true;
		}
	}

	return false;
}

bool UItemLockComponent::TryLock(AActor* LockingActor)
{
	// only lock if unlocked and we have a valid locking actor
	if (bUnlocked && IsValid(LockingActor))
	{
		// reset the unlocked flag
		bUnlocked = false;

		// call the delegate
		OnLocked.Broadcast(LockingActor);

		return true;
	}

	return false;
}