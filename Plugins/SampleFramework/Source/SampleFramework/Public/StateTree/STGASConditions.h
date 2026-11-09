// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "StateTreeTypes.h"
#include "STGASConditions.generated.h"

class AActor;

/**
 *  Instance data struct for FStateTreeGameplayTagsCondition
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeGameplayTagsConditionInstanceData
{
	GENERATED_BODY()
	
	/** Actor to check the gameplay tag on. The condition expects the actor to implement IAbilitySystemInterface and return a valid Ability System Component */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

};

/**
 *  StateTree condition to test the Actor's Ability System Component for a set of Gameplay Tags.
 *  The condition can check for multiple tags at once, do All or Any tag matches, and invert the result.
 */
USTRUCT(DisplayName = "Test Gameplay Ability Tags", Category = "GAS")
struct SAMPLEFRAMEWORK_API FStateTreeGameplayTagsCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	/** Tags to check for */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTagContainer Tags;

	/** If true, the condition tests for a match on all tags, otherwise it passes if any of the tags are present, including parents */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bMatchAll = true;

	/** If false, the condition passes if the character has the gameplay tags, otherwise passes if it doesn't have them */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;

	/** Set the instance data type */
	using FInstanceDataType = FStateTreeGameplayTagsConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Default constructor */
	FStateTreeGameplayTagsCondition() = default;
	
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
 *  Instance data struct for FStateTreeGameplayAttributeCondition
 */
USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeGameplayAttributeConditionInstanceData
{
	GENERATED_BODY()
	
	/** Actor to check the attribute on. The condition expects the actor to implement IAbilitySystemInterface and return a valid Ability System Component */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Value to test the attribute against */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float TestValue = 0.0f;

	/** Comparison operation to perform */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (InvalidEnumValues = "IsTrue"))
	UE::StateTree::EComparisonOperator Operator = UE::StateTree::EComparisonOperator::Equal;
};

/**
 *  StateTree condition to test a Gameplay Attribute's value on the target Actor.
 */
USTRUCT(DisplayName = "Test Gameplay Ability Attribute", Category = "GAS")
struct SAMPLEFRAMEWORK_API FStateTreeGameplayAttributeCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	/** Attribute to test */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayAttribute Attribute;

	/** If true, the condition passes if the comparison fails */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;

	/** Set the instance data type */
	using FInstanceDataType = FStateTreeGameplayAttributeConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Default constructor */
	FStateTreeGameplayAttributeCondition() = default;
	
	/** Tests the StateTree condition */
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	/** Provides the description string */
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
	
	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif

};