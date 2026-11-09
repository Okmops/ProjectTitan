// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "AttributeSet.h"
#include "STGASPropertyFunctions.generated.h"

USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeGetAttributePropertyFunctionInstanceData
{
	GENERATED_BODY()

	/** Actor to read the attribute on. Expects the actor to implement IAbilitySystemInterface and return a valid Ability System Component */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Retrieved attribute value */
	UPROPERTY(EditAnywhere, Category = "Output")
	float Result = 0.0f;
};

/**
 * Read a Gameplay Attribute value from the target Actor. 
 */
USTRUCT(meta = (DisplayName = "Get Attribute Value", Category = "GAS"))
struct SAMPLEFRAMEWORK_API FStateTreeGetAttributePropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	/** Attribute to get */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayAttribute Attribute;

	using FInstanceDataType = FStateTreeGetAttributePropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;

	/** Compile-time validations */
	virtual EDataValidationResult Compile(UE::StateTree::ICompileNodeContext& CompileContext) override;

#endif
};

////////////////////////////////////////////////////////////////////