// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

class UInventoryManagerComponent;

/**
 *  IInventoryInterface
 *  Helper interface to allow Actors such as Pawns to redirect to an Inventory Manager Component
 */
UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class INVENTORYSYSTEM_API IInventoryInterface
{
	GENERATED_BODY()

public:

	/** Returns the Inventory Manager Component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UInventoryManagerComponent* GetInventoryManagerComponent() const = 0;
};
