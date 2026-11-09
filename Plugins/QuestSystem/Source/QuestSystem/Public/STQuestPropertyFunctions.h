// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "GameplayTagContainer.h"
#include "STQuestPropertyFunctions.generated.h"

class AActor;

USTRUCT()
struct QUESTSYSTEM_API FStateTreeHasQuestTagPropertyFunctionInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to test. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Quest status tag to test. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestStatusTag;

	/** Output result */
	UPROPERTY(EditAnywhere, Category = "Output")
	bool bResult = false;
};

/**
 *  Returns true if the Target Actor has the given Quest Status Tag.
 *  Returns false if the Tag is invalid, the Actor does not have a valid Quest Manager Comp or the tag is not present.
 */
USTRUCT(meta = (DisplayName = "Has Quest Status Tag", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeHasQuestTagPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeHasQuestTagPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;

#endif
};

////////////////////////////////////////////////////////////////////

USTRUCT()
struct QUESTSYSTEM_API FStateTreeGetQuestQuantityPropertyFunctionInstanceData
{
	GENERATED_BODY()

	/** Actor holding the Quest Manager Component to test. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Quest quantity tag to test. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestQuantityTag;

	/** Output result */
	UPROPERTY(EditAnywhere, Category = "Output")
	int32 Result = 0;
};

/**
 *  Returns the count of the given Quest Quantity in the Target Actor.
 *  Returns 0 if the Tag is invalid or the Actor does not have a valid Quest Manager.
 */
USTRUCT(meta = (DisplayName = "Get Quest Quantity", Category = "Quest"))
struct QUESTSYSTEM_API FStateTreeGetQuestQuantityPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeGetQuestQuantityPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;

#endif
};

////////////////////////////////////////////////////////////////////