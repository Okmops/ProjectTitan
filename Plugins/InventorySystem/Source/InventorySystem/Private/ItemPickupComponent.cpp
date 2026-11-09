// Copyright Epic Games, Inc. All Rights Reserved.


#include "ItemPickupComponent.h"
#include "InventoryManagerComponent.h"
#include "Curves/CurveFloat.h"
#include "ItemDefinition.h"
#include "Fragments/ItemFragments_Pickup.h"
#include "InventoryInterface.h"
#include "GameFramework/Actor.h"

UItemPickupComponent::UItemPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UItemPickupComponent::PickUp(AActor* PickingActor)
{
	// ensure the item definition is valid
	if (Item)
	{
		// get the inventory manager comp
		UInventoryManagerComponent* Inventory = nullptr;

		// first try to get it through the inventory interface
		if (IInventoryInterface* InventoryInterface = Cast<IInventoryInterface>(PickingActor))
		{
			Inventory = InventoryInterface->GetInventoryManagerComponent();
		}

		// if we fail, search the actor for an inventory manager comp
		if (!Inventory)
		{
			// get the inventory manager comp
			Inventory = PickingActor->FindComponentByClass<UInventoryManagerComponent>();
		}

		// get the actor's inventory manager
		if (Inventory)
		{
			// check how many items the owner is holding
			int32 Held = Inventory->GetItemCount(Item->GetItemTag());

			// determine the amount to grant
			int32 GrantedAmount = Amount;

			// are we using a probability curve?
			if (AmountCurve)
			{
				// read the curve at a random probability value between 0 and 1
				const float rng = FMath::FRand();
				const float CurveValue = AmountCurve->GetFloatValue(rng);

				GrantedAmount = FMath::IsFinite(CurveValue) ? FMath::TruncToInt32(CurveValue) : 0;
			}

			// does the item definition have a Max Amount?
			if (const UItemFragment_MaxAmount* AmtFragment = Cast<UItemFragment_MaxAmount>(Item->FindFragmentByClass(UItemFragment_MaxAmount::StaticClass(), FGameplayTag::EmptyTag)))
			{
				// clamp the total amount to the max amount allowed amount for this item
				GrantedAmount = FMath::Min(GrantedAmount, AmtFragment->MaxAmount - Held);

				// if we can't grant at least one item, fail this pickup attempt
				if (GrantedAmount < 1)
				{
					return false;
				}
			}

			// attempt to grant the item
			if (Inventory->AddItem(Item->GetItemTag(), GrantedAmount))
			{
				// success, so call the delegate
				OnPickedUp.Broadcast(Item->GetItemTag(), GrantedAmount, PickingActor);

				return true;
			}
		}
	}

	return false;
}

TSoftObjectPtr<UStaticMesh> UItemPickupComponent::GetItemMesh() const
{
	// get the pickup fragment
	if (Item)
	{
		if (const UItemFragment_PickupData* PickupFragment = Cast<UItemFragment_PickupData>(Item->FindFragmentByClass(UItemFragment_PickupData::StaticClass(), FGameplayTag::EmptyTag)))
		{
			// return the pickup mesh
			return PickupFragment->PickupMesh;
		}
	}

	return nullptr;
}

float UItemPickupComponent::GetItemMeshScale() const
{
	// get the pickup fragment
	if (Item)
	{
		if (const UItemFragment_PickupData* PickupFragment = Cast<UItemFragment_PickupData>(Item->FindFragmentByClass(UItemFragment_PickupData::StaticClass(), FGameplayTag::EmptyTag)))
		{
			// return the pickup scale
			return PickupFragment->PickupScale;
		}
	}

	return 1.0f;
}

FVector UItemPickupComponent::GetCollisionSphereOffset() const
{
	// get the pickup fragment
	if (Item)
	{
		if (const UItemFragment_PickupData* PickupFragment = Cast<UItemFragment_PickupData>(Item->FindFragmentByClass(UItemFragment_PickupData::StaticClass(), FGameplayTag::EmptyTag)))
		{
			// return the offset
			return PickupFragment->CollisionSphereOffset;
		}
	}

	return FVector::ZeroVector;
}

float UItemPickupComponent::GetPickupRadius() const
{
	// get the pickup fragment
	if (Item)
	{
		if (const UItemFragment_PickupData* PickupFragment = Cast<UItemFragment_PickupData>(Item->FindFragmentByClass(UItemFragment_PickupData::StaticClass(), FGameplayTag::EmptyTag)))
		{
			// return the pickup radius
			return PickupFragment->PickupRadius;
		}
	}

	return 0.0f;
}

#if WITH_EDITOR

void UItemPickupComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// have we updated the item definition?
    if (PropertyChangedEvent.Property != nullptr)
    {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();

        if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemPickupComponent, Item))
        {
			// get the owning actor
			if (AActor* OwningActor = GetOwner())
			{
				// if the owning actor implements the interface, call the function
				if (OwningActor->GetClass()->ImplementsInterface(UPickupInterface::StaticClass()))
				{
					IPickupInterface::Execute_OnItemDefinitionChanged(OwningActor);
				}
			}
        }
    }
}

#endif // WITH_EDITOR