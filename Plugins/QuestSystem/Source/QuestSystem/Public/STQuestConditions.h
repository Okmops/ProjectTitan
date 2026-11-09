// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeConditionBase.h"
#include "STQuestConditions.generated.h"

class AActor;

/**
 *  EQuestStatusCheck
 *  Determines the Type of quest status check to perform
 */
UENUM()
enum class EQuestStatusCheck : uint8
{
	Active,
	Completed,
	Failed,

	MAX UMETA(Hidden)
};

/**
 *  Instance data struct for FQuestCompletionStatusCondition
 */
USTRUCT()
struct QUESTSYSTEM_API FQuestActiveConditionInstanceData
{
	GENERATED_BODY()
	
	/** Actor holding the Quest Manager Component to test. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Tag of the quest to test for. */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "QuestSystem.Quest"))
	FGameplayTag QuestTag;
};

/**
 *  StateTree condition to test if the Target Actor has a Quest active.
 *  Assumes the Target Actor to have a Quest Manager Component
 */
USTRUCT(DisplayName = "Test Quest Completion Status", Category = "Quest")
struct QUESTSYSTEM_API FQuestCompletionStatusCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	/** Type of quest completion status to check for */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	EQuestStatusCheck QuestStatusCheck = EQuestStatusCheck::Active;

	/** If false, the condition passes if the Quest status matches. If true, the condition passed if the Quest status doesn't match */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;

	/** Set the instance data type */
	using FInstanceDataType = FQuestActiveConditionInstanceData;
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

/**
 *  Instance data struct for FQuestStatusQueryCondition
 */
USTRUCT()
struct QUESTSYSTEM_API FQuestStatusQueryConditionInstanceData
{
	GENERATED_BODY()
	
	/** Actor holding the Quest Manager Component to test. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;
};

/**
 *  StateTree condition to perform a Gameplay Tag Query over the Quest Status Data on the given Actor.
 *  Assumes the Target Actor to have a Quest Manager Component
 */
USTRUCT(DisplayName = "Do Quest Status Query", Category = "Quest")
struct QUESTSYSTEM_API FQuestStatusQueryCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	/** Gameplay Tag Query to run */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTagQuery QuestStatusQuery;

	/** Set the instance data type */
	using FInstanceDataType = FQuestStatusQueryConditionInstanceData;
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