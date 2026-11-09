// Copyright Epic Games, Inc. All Rights Reserved.


#include "StateTree/STSamplePropertyFunctions.h"
#include "Math/UnrealMathUtility.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeNodeDescriptionHelpers.h"

#define LOCTEXT_NAMESPACE "StateTreeSamplePropertyFunctions"

void FStateTreeRandomFloatPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const float MinValue = FMath::Min(InstanceData.Min, InstanceData.Max);
	const float MaxValue = FMath::Max(InstanceData.Min, InstanceData.Max);

	InstanceData.Result = FMath::RandRange(MinValue, MaxValue);
}

#if WITH_EDITOR

FText FStateTreeRandomFloatPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType& InstanceData = InstanceDataView.Get<FInstanceDataType>();

	FText MinText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Min)), Formatting);
	if (MinText.IsEmpty())
	{
		MinText = UE::StateTree::DescHelpers::GetText(InstanceData.Min, Formatting);
	}

	FText MaxText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Max)), Formatting);
	if (MaxText.IsEmpty())
	{
		MaxText = UE::StateTree::DescHelpers::GetText(InstanceData.Max, Formatting);
	}

	return FText::Format(LOCTEXT("RandomFloat", "Random ({0} - {1})"), MinText, MaxText);
}

#endif //WITH_EDITOR

void FStateTreeRandomBoolPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bResult = FMath::RandBool();
}

#if WITH_EDITOR

FText FStateTreeRandomBoolPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return LOCTEXT("RandomBool", "Random Boolean");
}

#endif //WITH_EDITOR

void FStateTreeRandomVectorPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const float MinValue = FMath::Min(InstanceData.MinLength, InstanceData.MaxLength);
	const float MaxValue = FMath::Max(InstanceData.MinLength, InstanceData.MaxLength);

	const float Length = FMath::RandRange(MinValue, MaxValue);
	const FVector Dir = FMath::VRand();

	InstanceData.Result = (Dir.IsNearlyZero() ? FVector::ForwardVector : Dir) * Length;
}

#if WITH_EDITOR

FText FStateTreeRandomVectorPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType& InstanceData = InstanceDataView.Get<FInstanceDataType>();

	FText MinText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, MinLength)), Formatting);
	if (MinText.IsEmpty())
	{
		MinText = UE::StateTree::DescHelpers::GetText(InstanceData.MinLength, Formatting);
	}

	FText MaxText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, MaxLength)), Formatting);
	if (MaxText.IsEmpty())
	{
		MaxText = UE::StateTree::DescHelpers::GetText(InstanceData.MaxLength, Formatting);
	}

	return FText::Format(LOCTEXT("RandomVector", "Random Vector with Length ({0} - {1})"), MinText, MaxText);
}

#endif //WITH_EDITOR

#undef LOCTEXT_NAMESPACE