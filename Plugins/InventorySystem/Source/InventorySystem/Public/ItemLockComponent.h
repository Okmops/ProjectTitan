// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ItemLockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemLockDelegate, AActor*, InteractingActor);

/**
 *  FKeyItemDescription
 *  Defines the type and amount of items needed to unlock the component
 */
USTRUCT(BlueprintType)
struct FKeyItemDescription
{
	GENERATED_BODY()

	/** Tag of the item needed to unlock */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (Categories = "Inventory.Item"))
	FGameplayTag ItemTag;

	/** Amount of the item needed to satisfy the lock */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 1))
	int32 Amount = 1;
};

/**
 *  UItemLockComponent
 *  Leverages the Inventory System to establish an item-based lock.
 *  To open the lock, the unlocking actor must hold the required types and amounts of key items.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UItemLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	/** Constructor */
	UItemLockComponent();

protected:

	/** List of key items needed to unlock */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lock")
	TArray<FKeyItemDescription> KeyItems;

	/** If true, this lock is currently unlocked */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lock")
	bool bUnlocked = false;

	/** If true, key items are consumed from the interacting actor's inventory when unlocking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lock")
	bool bConsumeItemsOnUnlock = true;

	/** Delegate to trigger when the lock is unlocked */
	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FItemLockDelegate OnUnlocked;

	/** Delegate to trigger when the lock is locked */
	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FItemLockDelegate OnLocked;

public:

	/** Checks the unlocking actor's inventory for the required items and attempts to unlock the component. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool TryUnlock(AActor* UnlockingActor);
	
	/** Resets the component to locked state if unlocked. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool TryLock(AActor* LockingActor);
};