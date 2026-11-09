// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "STInventoryTasks.generated.h"

class AActor;

/**
 *  Instance data struct for StateTree Item Tasks
 */
USTRUCT()
struct INVENTORYSYSTEM_API FStateTreeItemTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Inventory Manager Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Tag of the item to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "Inventory.Item"))
	FGameplayTag ItemTag;

	/** Number of items to affect. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 1))
	int32 Count = 1;
};

/**
 *  StateTree Task to give an Item to the Target Actor
 *  Assumes the Target Actor to have an Inventory Manager Component
 *  Succeeds if the item is successfully added, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Give Item", Category = "Inventory"))
struct INVENTORYSYSTEM_API FStateTreeGiveItemTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeGiveItemTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeItemTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  StateTree Task to remove an Item to the Target Actor
 *  Assumes the Target Actor to have an Inventory Manager Component
 *  Succeeds if the item is successfully removed, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Remove Item", Category = "Inventory"))
struct INVENTORYSYSTEM_API FStateTreeRemoveItemTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeRemoveItemTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeItemTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  StateTree Task to check if the Target Actor has the required number of items.
 *  Assumes the Target Actor to have an Inventory Manager Component
 *  Succeeds if the target item count is present, fails otherwise
 */
USTRUCT(meta=(DisplayName="Check Item Count", Category = "Inventory"))
struct INVENTORYSYSTEM_API FStateTreeCheckItemCountTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeCheckItemCountTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeItemTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for FStateTreeActivateItemTask
 */
USTRUCT()
struct INVENTORYSYSTEM_API FStateTreeActivateItemTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Inventory Manager Component to query. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> ItemOwner = nullptr;

	/** Target for the item. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> ItemTarget = nullptr;

	/** Tag of the item to activate. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "Inventory.Item"))
	FGameplayTag ItemTag;
};

/**
 *  StateTree Task to activate an Item on the Target Actor
 *  Assumes the Target Actor to have an Inventory Manager Component
 *  If an Item Target is not provided, the Item Owner will be targeted by the item.
 *  Succeeds if the item is successfully activated, fails otherwise.
 */
USTRUCT(meta=(DisplayName="Activate Item", Category = "Inventory"))
struct INVENTORYSYSTEM_API FStateTreeActivateItemTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeActivateItemTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeActivateItemTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif // WITH_EDITOR
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for FStateTreeOperateLockTask
 */
USTRUCT()
struct INVENTORYSYSTEM_API FStateTreeOperateLockTaskInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Item Lock Component to affect. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> LockActor = nullptr;

	/** Actor that will interact the Lock. Should have an Inventory Manager Component. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> InteractingActor = nullptr;

	/** If true, the task will attempt to Unlock using the Unlocking Actor's inventory. Otherwise it will attempt to Lock the component if it's unlocked. */
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bUnlock = true;
};

/**
 *  StateTree Task to control the ItemLockComponent on a given Actor.
 *  You can either attempt to Unlock the component (requires an Unlocking Actor with an Inventory Manager Component), or Lock it again if it's already been unlocked.
 *  The task succeeds if the operation is successful, fails if the attempt failed or requirements are not met.
 */
USTRUCT(meta=(DisplayName="Operate Lock", Category = "Inventory"))
struct INVENTORYSYSTEM_API FStateTreeOperateLockTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Constructor */
	FStateTreeOperateLockTask()
	{
		// no tick needed
		bShouldCallTick = false;

		// skip state change events if this is sustained
		bShouldStateChangeOnReselect = false;
	}

	/* Ensure we're using the correct instance data struct */
	using FInstanceDataType = FStateTreeOperateLockTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	/** Details panel description*/
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

#endif // WITH_EDITOR
};