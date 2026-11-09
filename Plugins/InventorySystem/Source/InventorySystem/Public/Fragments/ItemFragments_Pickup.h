// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "ItemFragments_Pickup.generated.h"

class UStaticMesh;

/**
 *  UItemFragment_PickupData
 *  Holds information about a pickup item.
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_PickupData : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** Pickup Mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TSoftObjectPtr<UStaticMesh> PickupMesh;

	/** Pickup radius for this item in cm */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 0, Units = "cm"))
	float PickupRadius = 32.0f;

	/** Offset of the collision sphere relative to the pickup mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	FVector CollisionSphereOffset;

	/** Scale to apply to the pickup mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 0))
	float PickupScale = 1.0f;

	/** Text used when picking up a single instance of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	FText SingularPickupText;

	/** Text used when picking up multiple instances of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	FText PluralPickupText;

};

///////////////////////////////////////////////////////

/**
 *  UItemFragment_MaxAmount
 *  Holds information about the max amount of stacks of an item allowed in an inventory.
 */
UCLASS()
class INVENTORYSYSTEM_API UItemFragment_MaxAmount : public UItemFragment
{
	GENERATED_BODY()
	
public:

	/** Max Amount of items allowed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Max Amount", meta = (ClampMin = 1))
	int32 MaxAmount = 1;

};