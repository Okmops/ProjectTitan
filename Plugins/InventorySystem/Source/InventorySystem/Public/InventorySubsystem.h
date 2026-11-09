// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "InventorySubsystem.generated.h"

class UItemDefinition;

/**
 *  UInventorySubsystem
 *  Maintains an index of all available UItemDefinition assets and their associated tags. 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:

	/** Item Definitions map, indexed by tag */
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UItemDefinition>> ItemDefinitions;

public:

	/** Builds the Item Definition Map on initialization */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Clears the Item Definition Map on deinitialization */
	virtual void Deinitialize() override;

protected:

	UFUNCTION()
	void BuildItemMap();

public:

	/** Finds an item definition with the given tag. Returns nullptr if no definition is found. */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	UItemDefinition* GetItemDefinition(FGameplayTag ItemTag) const;

};
