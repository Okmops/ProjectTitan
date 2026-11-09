// Copyright Epic Games, Inc. All Rights Reserved.


#include "STInventoryConditions.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeTypes.h"
#include "StateTreePropertyBindings.h"
#include "StateTreeNodeDescriptionHelpers.h"
#include "StateTreeNodeBase.h"
#include "InventorySystemLogging.h"
#include "InventoryManagerComponent.h"
#include "InventoryInterface.h"

#define LOCTEXT_NAMESPACE "StateTreeInventoryConditions"

bool FStateTreeHasItemCountCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the item tag is not valid
	if (!InstanceData.ItemTag.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeHasItemCountCondition: Item Tag is not valid."));
		return false;
	}

	// fail if the count is less than 1
	if (InstanceData.Count < 1)
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeHasItemCountCondition: Item Count is not valid."));
		return false;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeHasItemCountCondition: Target Actor is not valid."));
		return false;
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
		UE_LOG(LogInventory, Error, TEXT("FStateTreeHasItemCountCondition: Target Actor does not have an Inventory Manager Component."));
		return false;
	}

	// do we have the required item count?
	const bool bHasItems = Inventory->GetItemCount(InstanceData.ItemTag) >= InstanceData.Count;

	return bInvert ? !bHasItems : bHasItems;
}

#if WITH_EDITOR

FText FStateTreeHasItemCountCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("HasItemCountConditionInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
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
			return LOCTEXT("HasItemCountConditionInvalidTag", "<b>WARNING: Invalid Item Tag. Check the Item Tag Binding.</>");
		}
	}

	// get the name of the count binding
	FText CountText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Count)), Formatting);
	if (CountText.IsEmpty())
	{
		if (InstanceData->Count > 0)
		{
			CountText = FText::AsNumber(InstanceData->Count);
		}
		else
		{
			return LOCTEXT("HasItemCountConditionInvalidCount", "<b>WARNING: Invalid Item Count. Check the Count Binding.</>");
		}
	}

	// format and output the description
	return bInvert ? FText::Format(LOCTEXT("HasItemCountConditionTargetInv", "Actor <b>{0}</> does not have at least <b>{1}</> items of type <b>{2}</>"), TargetText, CountText, ItemText)
		: FText::Format(LOCTEXT("HasItemCountConditionTarget", "Actor <b>{0}</> has at least <b>{1}</> items of type <b>{2}</>"), TargetText, CountText, ItemText);
}

EDataValidationResult FStateTreeHasItemCountCondition::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the item tag is valid
	if (!InstanceData.ItemTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("HasItemCountConditionErrorInvalidTag", "Invalid Item Tag. Check the Item Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE