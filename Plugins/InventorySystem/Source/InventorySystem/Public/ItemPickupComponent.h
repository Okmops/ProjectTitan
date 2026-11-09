// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ItemPickupComponent.generated.h"

class UCurveFloat;
class UItemDefinition;
class UStaticMesh;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPickupDelegate, const FGameplayTag&, ItemTag, int32, Amount, AActor*, PickingActor);

/**
 *  IPickupInterface
 *  Allows Actors to react to item definition changes in their Pickup Components
 *  Workaround to Construction Script not supporting component property reads
 */
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class INVENTORYSYSTEM_API IPickupInterface
{
	GENERATED_BODY()

public:

	/** Called when the Item Definition has changed in a pickup component owned by this Actor */
	UFUNCTION(BlueprintImplementableEvent, CallInEditor)
	void OnItemDefinitionChanged();
};


/**
 *  UItemPickupComponent
 *  Leverages the Inventory System to grant an amount of an item to an Actor.
 */
UCLASS( ClassGroup = ("Inventory"), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UItemPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	/** Constructor */
	UItemPickupComponent();

protected:

	/** Item to pick up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UItemDefinition> Item;

	/** Amount of the item to grant on pickup if no probability curve is available */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 1))
	int32 Amount = 1;

	/** Optional probability curve to determine the amount of items to grant on pickup */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UCurveFloat> AmountCurve;

	/** Delegate to trigger when the item was successfully picked up */
	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FPickupDelegate OnPickedUp;

public:

	/** Attempts to grant the item associated by this item to the specified actor. Does nothing if the actor does not have an Inventory Component */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool PickUp(AActor* PickingActor);

	/** Attempts to return the mesh associated with the pickup item */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	TSoftObjectPtr<UStaticMesh> GetItemMesh() const;

	/** Attempts to return the mesh scale associated with the pickup item */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	float GetItemMeshScale() const;

	/** Attempts to return the collision sphere offset associated with the pickup item */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FVector GetCollisionSphereOffset() const;

	/** Attempts to return the pickup radius for the associated item */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	float GetPickupRadius() const;

public:

#if WITH_EDITOR

	/** Allow the owner to update its appearance in response to item changes */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif // WITH_EDITOR

};