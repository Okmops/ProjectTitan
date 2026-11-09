// Copyright Epic Games, Inc. All Rights Reserved.


#include "InventoryFunctionLibrary.h"
#include "InventorySubsystem.h"
#include "ItemDefinition.h"
#include "InventorySystemLogging.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UItemDefinition* UInventoryFunctionLibrary::GetItemDefinition(const UObject* WorldContextObject, FGameplayTag ItemTag)
{
	// get the inventory subsystem from the world
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UInventorySubsystem* InventorySubsystem = World->GetSubsystem<UInventorySubsystem>())
		{
			// get the item definition from the inventory subsystem
			return InventorySubsystem->GetItemDefinition(ItemTag);
		}
		else
		{
			UE_LOG(LogInventory, Error, TEXT("UInventoryFunctionLibrary: Couldn't resolve the Inventory World Subsystem."));
		}
	}
	else
	{
		UE_LOG(LogInventory, Error, TEXT("UInventoryFunctionLibrary: Couldn't resolve the World from the WorldContextObject."));
	}

	return nullptr;
}
