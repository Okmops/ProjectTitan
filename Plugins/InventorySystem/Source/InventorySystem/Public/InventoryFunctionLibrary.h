// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "InventoryFunctionLibrary.generated.h"

class UItemDefinition;

/**
 *  UInventoryFunctionLibrary
 *  Utility functions to interact with the Inventory System
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/** Retrieves an item definition from the Inventory Subsystem if it exists */
	UFUNCTION(BlueprintPure, Category = "Inventory System", meta=(WorldContext="WorldContextObject"))
	static UItemDefinition* GetItemDefinition(const UObject* WorldContextObject, FGameplayTag ItemTag);

};
