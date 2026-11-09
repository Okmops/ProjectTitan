// Copyright Epic Games, Inc. All Rights Reserved.


#include "STInventoryPropertyFunctions.h"
#include "StateTreeExecutionContext.h"
#include "InventoryManagerComponent.h"
#include "InventorySystemLogging.h"
#include "InventoryInterface.h"

#define LOCTEXT_NAMESPACE "StateTreeInventoryPropertyFunctions"

void FStateTreeGetItemCountPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// assume the result is 0 by default
	InstanceData.Result = 0;

	// fail if the item tag is not valid
	if (!InstanceData.ItemTag.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeGetItemCountPropertyFunction: Item Tag is not valid."));
		return;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeGetItemCountPropertyFunction: Target Actor is not valid."));
		return;
	}

	// get the inventory manager comp
	UInventoryManagerComponent* Inventory = nullptr;

	// first try to get it through the inventory interface
	if (IInventoryInterface* InventoryInterface = Cast<IInventoryInterface>(InstanceData.TargetActor))
	{
		Inventory = InventoryInterface->GetInventoryManagerComponent();
	}

	// if we fail, search the actor for an inventory manager comp
	if (!Inventory)
	{
		// get the inventory manager comp
		Inventory = InstanceData.TargetActor->FindComponentByClass<UInventoryManagerComponent>();
	}

	if (!Inventory)
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeGetItemCountPropertyFunction: Target Actor does not have an Inventory Manager Component."));
		return;
	}

	// return the item tag's count
	InstanceData.Result = Inventory->GetItemCount(InstanceData.ItemTag);
}

#if WITH_EDITOR

FText FStateTreeGetItemCountPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GetItemCountPFInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// get the name of the item tag binding
	FText ItemText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemTag)), Formatting);
	if (ItemText.IsEmpty())
	{
		// is the item tag valid?
		if (InstanceData->ItemTag.IsValid())
		{
			// use the tag itself
			ItemText = FText::FromString(InstanceData->ItemTag.ToString());
		}
		else
		{
			return LOCTEXT("GetItemCountPFInvalidTag", "<b>WARNING: Invalid Item Tag. Check the Item Tag Binding.</>");
		}
	}

	// format and output the description
	return FText::Format(LOCTEXT("GetItemCountPFTarget", "Number of Items with tag <b>{0}</> on Actor <b>{1}</>"), ItemText, TargetText);
}

#endif

#undef LOCTEXT_NAMESPACE