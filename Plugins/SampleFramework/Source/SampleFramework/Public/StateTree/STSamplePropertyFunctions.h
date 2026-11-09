// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "STSamplePropertyFunctions.generated.h"

USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeRandomFloatPropertyFunctionInstanceData
{
	GENERATED_BODY()

	/** Min bound */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float Min = 0.0f;

	/** Max bound */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float Max = 0.0f;

	/** Generated random value */
	UPROPERTY(EditAnywhere, Category = "Output")
	float Result = 0.0f;
};

/**
 * Generate a random float value between a Min and a Max bound
 */
USTRUCT(meta = (DisplayName = "Random Float in Range", Category = "Math|Random"))
struct SAMPLEFRAMEWORK_API FStateTreeRandomFloatPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeRandomFloatPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;

#endif
};

////////////////////////////////////////////////////////////////////

USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeRandomBoolPropertyFunctionInstanceData
{
	GENERATED_BODY()

	/** Generated random value */
	UPROPERTY(EditAnywhere, Category = "Output")
	bool bResult = false;
};

/**
 * Generate a random boolean
 */
USTRUCT(meta = (DisplayName = "Random Bool", Category = "Math|Random"))
struct SAMPLEFRAMEWORK_API FStateTreeRandomBoolPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeRandomBoolPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;

#endif
};

////////////////////////////////////////////////////////////////////

USTRUCT()
struct SAMPLEFRAMEWORK_API FStateTreeRandomVectorPropertyFunctionInstanceData
{
	GENERATED_BODY()

	/** Min length bound */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 0))
	float MinLength = 1.0f;

	/** Max length bound */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = 0))
	float MaxLength = 1.0f;

	/** Generated random vector */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector Result = FVector::ZeroVector;
};

/**
 * Generate a random vector with the given length range
 */
USTRUCT(meta = (DisplayName = "Random Vector", Category = "Math|Random"))
struct SAMPLEFRAMEWORK_API FStateTreeRandomVectorPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeRandomVectorPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override;

#endif
};