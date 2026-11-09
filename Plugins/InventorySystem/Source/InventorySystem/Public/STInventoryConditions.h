// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeConditionBase.h"
#include "STInventoryConditions.generated.h"

class AActor;

/**
 *  Instance data struct for FStateTreeHasItemCountCondition
 */
USTRUCT()
struct INVENTORYSYSTEM_API FStateTreeHasItemCountConditionInstanceData
{
	GENERATED_BODY()
	
	/** Actor holding the Inventory Manager Component to test. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Tag of the item to test. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "Inventory.Item"))
	FGameplayTag ItemTag;

	/** Minimum number of items to pass. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 1))
	int32 Count = 1;
};

/**
 *  StateTree condition to test if the Target Actor has at least the specified number of items in their inventory.
 *  Assumes the Target Actor to have an Inventory Manager Component
 */
USTRUCT(DisplayName = "Has Item Count", Category = "Inventory")
struct INVENTORYSYSTEM_API FStateTreeHasItemCountCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	/** If false, the condition passes if the Actor has the required items, otherwise passes if it doesn't have them */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;

	/** Set the instance data type */
	using FInstanceDataType = FStateTreeHasItemCountConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
		
	/** Tests the StateTree condition */
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	/** Provides the description string */
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif

};

////////////////////////////////////////////////////////////////////