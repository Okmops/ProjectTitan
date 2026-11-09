// Copyright Epic Games, Inc. All Rights Reserved.


#include "STInventoryTasks.h"
#include "StateTreeExecutionContext.h"
#include "InventorySystemLogging.h"
#include "InventoryManagerComponent.h"
#include "InventoryInterface.h"
#include "ItemDefinition.h"
#include "InventoryFunctionLibrary.h"
#include "ItemLockComponent.h"
#include "GameFramework/Actor.h"

#define LOCTEXT_NAMESPACE "StateTreeInventoryTasks"

EStateTreeRunStatus FStateTreeGiveItemTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the item tag is not valid
	if (!InstanceData.ItemTag.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeGiveItemTask: Item Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the count is less than 1
	if (InstanceData.Count < 1)
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeGiveItemTask: Item Count is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor does not have a valid inventory interface
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeGiveItemTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogInventory, Error, TEXT("FStateTreeGiveItemTask: Target Actor does not have an Inventory Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// attempt to grant the item, report success or failure depending on the result
	if(Inventory->AddItem(InstanceData.ItemTag, InstanceData.Count))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}
}

#if WITH_EDITOR

FText FStateTreeGiveItemTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GiveItemTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
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
			return LOCTEXT("GiveItemTaskInvalidTag", "<b>WARNING: Invalid Item Tag. Check the Item Tag Binding.</>");
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
			return LOCTEXT("GiveItemTaskInvalidCount", "<b>WARNING: Invalid Item Count. Check the Count Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("GiveItemTaskTarget", "Give Item <b>{0}</> to Actor <b>{1}</>"), ItemText, TargetText));

	OutText.AppendLine(FText::Format(LOCTEXT("GiveItemTaskCount", "<s>Count: </> {0}"), CountText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeGiveItemTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the item tag is valid
	if (!InstanceData.ItemTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("GiveItemTaskErrorInvalidTag", "Invalid Item Tag. Check the Item Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeRemoveItemTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the item tag is not valid
	if (!InstanceData.ItemTag.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeRemoveItemTask: Item Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the count is less than 1
	if (InstanceData.Count < 1)
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeRemoveItemTask: Item Count is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeRemoveItemTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogInventory, Error, TEXT("FStateTreeRemoveItemTask: Target Actor does not have an Inventory Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// attempt to remove the item, report success or failure depending on the result
	if(Inventory->RemoveItem(InstanceData.ItemTag, InstanceData.Count))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeRemoveItemTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("RemoveItemTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
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
			return LOCTEXT("RemoveItemTaskInvalidTag", "<b>WARNING: Invalid Item Tag. Check the Item Tag Binding.</>");
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
			return LOCTEXT("RemoveItemTaskInvalidCount", "<b>WARNING: Invalid Item Count. Check the Count Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("RemoveItemTaskTarget", "Remove Item <b>{0}</> from Actor <b>{1}</>"), ItemText, TargetText));

	OutText.AppendLine(FText::Format(LOCTEXT("RemoveItemTaskCount", "<s>Count: </> {0}"), CountText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeRemoveItemTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the item tag is valid
	if (!InstanceData.ItemTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("RemoveItemTaskErrorInvalidTag", "Invalid Item Tag. Check the Item Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeCheckItemCountTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the item tag is not valid
	if (!InstanceData.ItemTag.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeCheckItemCountTask: Item Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the count is less than 1
	if (InstanceData.Count < 1)
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeCheckItemCountTask: Item Count is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeCheckItemCountTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogInventory, Error, TEXT("FStateTreeCheckItemCountTask: Target Actor does not have an Inventory Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// check if the inventory has at least that many items
	if(Inventory->GetItemCount(InstanceData.ItemTag) >= InstanceData.Count)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeCheckItemCountTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("CheckItemCountTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
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
			return LOCTEXT("CheckItemCountTaskInvalidTag", "<b>WARNING: Invalid Item Tag. Check the Item Tag Binding.</>");
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
			return LOCTEXT("CheckItemCountTaskInvalidCount", "<b>WARNING: Invalid Item Count. Check the Count Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("CheckItemCountTaskTarget", "Actor <b>{0}</> has at least {1} items of type {2}"), TargetText, CountText, ItemText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeCheckItemCountTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the item tag is valid
	if (!InstanceData.ItemTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("CheckItemCountTaskErrorInvalidTag", "Invalid Item Tag. Check the Item Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeActivateItemTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the item tag is not valid
	if (!InstanceData.ItemTag.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeActivateItemTask: Item Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.ItemOwner))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeActivateItemTask: Item Owner is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// get the inventory manager comp
	UInventoryManagerComponent* Inventory = nullptr;

	// first try to get it through the inventory interface
	if (IInventoryInterface* InventoryInterface = Cast<IInventoryInterface>(InstanceData.ItemOwner))
	{
		Inventory = InventoryInterface->GetInventoryManagerComponent();
	}

	// if we fail, search the actor for an inventory manager comp
	if (!Inventory)
	{
		// get the inventory manager comp
		Inventory = InstanceData.ItemOwner->FindComponentByClass<UInventoryManagerComponent>();
	}

	if (!Inventory)
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeActivateItemTask: Item Owner does not have an Inventory Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	if (!Inventory->HasItem(InstanceData.ItemTag))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeActivateItemTask: Item Owner does not have the Item [%s]."), *InstanceData.ItemTag.ToString());
		return EStateTreeRunStatus::Failed;
	}

	// get the item definition
	UItemDefinition* ItemDef = UInventoryFunctionLibrary::GetItemDefinition(Inventory, InstanceData.ItemTag);

	if (!IsValid(ItemDef))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeActivateItemTask: No item definition attached to item with Tag [%s]."), *InstanceData.ItemTag.ToString());
		return EStateTreeRunStatus::Failed;
	}

	AActor* ItemTarget = IsValid(InstanceData.ItemTarget) ? InstanceData.ItemTarget : InstanceData.ItemOwner;

	// activate the item
	if (ItemDef->ActivateItem(InstanceData.ItemOwner, ItemTarget))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeActivateItemTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the item owner binding
	FText OwnerText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemOwner)), Formatting);
	if (OwnerText.IsEmpty())
	{
		return LOCTEXT("ActivateItemTaskInvalidOwner", "<b>WARNING: Invalid Item Owner. Check the Item Owner Binding.</>");
	}

	// get the name of the item target binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemTarget)), Formatting);
	if (TargetText.IsEmpty())
	{
		// do we have a valid pointer to a specific asset?
		if (IsValid(InstanceData->ItemTarget))
		{
			TargetText = FText::FromString(InstanceData->ItemTarget->GetName());
		}
		else
		{
			// default to the owner
			TargetText = FText::Format(LOCTEXT("ActivateItemTaskDefaultTarget", "{0} (set by default if target is null)"), OwnerText);
		}
		
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
			return LOCTEXT("ActivateItemTaskInvalidTag", "<b>WARNING: Invalid Item Tag. Check the Item Tag Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("ActivateItemTaskActivate", "Activate Item <b>{0}</> on Actor <b>{1}</>"), ItemText, OwnerText));

	OutText.AppendLine(FText::Format(LOCTEXT("ActivateItemTaskTarget", "<s>Target: </> {0}"), TargetText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeActivateItemTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the item tag is valid
	if (!InstanceData.ItemTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ItemTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("ActivateItemTaskErrorInvalidTag", "Invalid Item Tag. Check the Item Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeOperateLockTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the lock actor is not valid
	if (!IsValid(InstanceData.LockActor))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeOperateLockTask: Lock Actor is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// get the item lock comp. 
	UItemLockComponent* Lock = InstanceData.LockActor->FindComponentByClass<UItemLockComponent>();

	if (!IsValid(Lock))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeOperateLockTask: Lock Actor does not have an Item Lock Component."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the interacting actor is not valid
	// No need to get the inventory, the lock comp will do it for us.
	if (!IsValid(InstanceData.InteractingActor))
	{
		UE_LOG(LogInventory, Error, TEXT("FStateTreeOperateLockTask: Interacting Actor is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// are we attempting an unlock?
	if (InstanceData.bUnlock)
	{
		// try to unlock, succeed if unlocked, fail otherwise
		if (Lock->TryUnlock(InstanceData.InteractingActor))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		// try to lock, succeed if locked, fail otherwise
		if (Lock->TryLock(InstanceData.InteractingActor))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeOperateLockTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the lock actor binding
	FText LockText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, LockActor)), Formatting);
	if (LockText.IsEmpty())
	{
		return LOCTEXT("OperateLockTaskInvalidLock", "<b>WARNING: Invalid Lock Actor. Check the Lock Actor Binding.</>");
	}

	// get the name of the interacting actor binding
	FText InteractingText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, InteractingActor)), Formatting);
	if (InteractingText.IsEmpty())
	{
		return LOCTEXT("OperateLockTaskInvalidInteractor", "<b>WARNING: Invalid Interacting Actor. Check the Interacting Actor Binding.</>");
	}

	// format and output the description
	return InstanceData->bUnlock ? FText::Format(LOCTEXT("OperateLockTaskUnlock", "Try to have Actor <b>{0}</> <b>Unlock</> Actor <b>{1}</>"), InteractingText, LockText)
	 : FText::Format(LOCTEXT("OperateLockTaskLock", "Try to have Actor <b>{0}</> <b>Lock</> Actor <b>{1}</>"), InteractingText, LockText);
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE