// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "GameplayTagContainer.h"
#include "STInventoryPropertyFunctions.generated.h"

class AActor;

USTRUCT()
struct INVENTORYSYSTEM_API FStateTreeGetItemCountPropertyFunctionInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Inventory Manager Component to test. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Tag of the item to test. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "Inventory.Item"))
	FGameplayTag ItemTag;

	/** Output item count */
	UPROPERTY(EditAnywhere, Category = "Output")
	int32 Result = 0;
};

/**
 *  Returns the amount of the given item held in the Target Actor's Inventory Manager Component.
 *  Returns 0 if the Tag is invalid or the Actor does not have a valid Inventory Manager Comp.
 */
USTRUCT(meta = (DisplayName = "Get Item Count", Category = "Inventory"))
struct INVENTORYSYSTEM_API FStateTreeGetItemCountPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeGetItemCountPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;

#endif
};

////////////////////////////////////////////////////////////////////