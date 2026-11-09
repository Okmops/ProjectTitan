// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "ItemFragments_UI.generated.h"

class UMaterialInterface;

/**
 *  UItemFragment_Icons
 *  Item data used to construct item icons in the UI.
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_Icons : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** Large icon used in menus. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Icon Data")
	TSoftObjectPtr<UMaterialInterface> LargeIcon;

	/** Small icon used in menus. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Icon Data")
	TSoftObjectPtr<UMaterialInterface> SmallIcon;
};

///////////////////////////////////////////////////////

/**
 *  UItemFragment_Description
 *  Item data used to display detailed icon information in the UI
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_Description : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** User-facing item name. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description Data")
	FText ItemName;

	/** Extended item description. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description Data")
	FText ItemDescription;

	/** Descriptive image of the item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description Data")
	TSoftObjectPtr<UMaterialInterface> ItemImage;

};

///////////////////////////////////////////////////////