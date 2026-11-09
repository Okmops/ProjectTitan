// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "UObject/ObjectMacros.h"
#include "InventoryManagerComponent.generated.h"

/** Root tag for all inventory items */
INVENTORYSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Inventory_Item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInventoryItemCountChangedDelegate, const FGameplayTag&, ItemTag, int32, CountDelta, int32, TotalCount);

/**
 *  UInventoryManagerComponent
 *  Manages an inventory through a collection of Gameplay Tag Stacks
 *  Each Tag can be associated with an item type through UItemDefinitions
 *  NOTE: This system does not support replication!
 */
UCLASS( ClassGroup = ("Inventory System"), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UInventoryManagerComponent();

protected:

	/** Container that holds the gameplay tags and counts for each item. */
	TMap<FGameplayTag, int32> ItemStacks;

public:

	/** Called when an item has been added or removed from the inventory */
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FInventoryItemCountChangedDelegate OnItemCountChanged;

public:

	/** Returns the entire item tag stacks container. */
	const TMap<FGameplayTag, int32>& GetItemStacks() const;

	/** Updates the entire item tag stacks container with the passed one. */
	void UpdateItemStacks(const TMap<FGameplayTag, int32>& Items);

	/** Adds an amount of the given item to the container. Ignored if Count <= 0 */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(FGameplayTag ItemTag, int32 Count = 1);

	/** Removes an amount of the given item from the container. Ignored if Count <= 0 */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(FGameplayTag ItemTag, int32 Count = 1);

	/** Returns true if the inventory holds at least one item with the given tag. */
	UFUNCTION(BlueprintPure, Category="Inventory")
	bool HasItem(FGameplayTag ItemTag) const;

	/** Returns the number of stacks for a given item tag. Matches are exact; you can't get aggregated counts for item hierarchies with this. */
	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetItemCount(FGameplayTag ItemTag) const;

	/** Populates an array of all held item tags that are children of the given parent tag. */
	UFUNCTION(BlueprintPure, Category="Inventory")
	void GetAllChildItemTags(FGameplayTag ParentTag, TArray<FGameplayTag>& OutTags) const;

protected:

	/** Utility to determine if the given tag represents an item. */
	bool IsItemTag(const FGameplayTag& ItemTag) const;
};