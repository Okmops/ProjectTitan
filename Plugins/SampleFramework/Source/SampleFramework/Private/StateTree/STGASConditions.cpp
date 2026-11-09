// Copyright Epic Games, Inc. All Rights Reserved.

#include "StateTree/STGASConditions.h"
#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"
#include "SampleLogging.h"
#include "StateTreeNodeDescriptionHelpers.h"
#include "Conditions/StateTreeCommonConditions.h"
#include "AbilitySystemGlobals.h"

#define LOCTEXT_NAMESPACE "StateTreeGASConditions"

////////////////////////////////////////////////////////////////////

bool FStateTreeGameplayTagsCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return false;
	}

	bool bTest = false;

	// ensure we don't test against no tags
	if (Tags.IsValid())
	{
		// test for all or any tags
		bTest = bMatchAll ? ASC->HasAllMatchingGameplayTags(Tags) : ASC->HasAnyMatchingGameplayTags(Tags);
	}

	// return the test result
	return bInvert ? !bTest : bTest;	
}

#if WITH_EDITOR

FText FStateTreeGameplayTagsCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GameplayTagsConditionInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");	
	}

	if (!Tags.IsValid())
	{
		return LOCTEXT("GameplayTagsConditionInvalidTags", "<b>WARNING: No Tags specified. Add at least one Tag to test for.</>");	
	}

	// get the match type text
	FText MatchText = bMatchAll ? LOCTEXT("GameplayTagsConditionExactTags", "<b>ALL</>") : LOCTEXT("GameplayTagsConditionAnyTags", "<b>ANY (May Include Parents)</>");

	// build the description text
	FTextBuilder OutText;

	// do we have inverted results?
	if (bInvert)
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayTagsConditionHasNoTags", "{0} Does Not Have {1}"), TargetText, MatchText));
	}
	else
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayTagsConditionHasTags", "{0} Has {1}"), TargetText, MatchText));
	}

	// append the tags
	OutText.AppendLine(LOCTEXT("GameplayTagsConditionTagsStart", "<b>Tags</>\n{"));

	for (const FGameplayTag& CurrentTag : Tags)
	{
		OutText.AppendLine(FText::Format(LOCTEXT("GameplayTagsConditionTagsLine", "   {0}"), FText::FromString(CurrentTag.ToString())));
	}

	OutText.AppendLine(LOCTEXT("GameplayTagsConditionTagsEnd", "}"));


	// return the description
	return OutText.ToText();
}

EDataValidationResult FStateTreeGameplayTagsCondition::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure we have at least tag to check against
	if (!Tags.IsValid())
	{
		CompileContext.AddValidationError(LOCTEXT("GameplayTagsConditionErrorMissingTags", "No Tags specified. Add at least one Tag to test for."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif

////////////////////////////////////////////////////////////////////

bool FStateTreeGameplayAttributeCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		return false;
	}

	// ensure the attribute is valid
	if (!Attribute.IsValid())
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGameplayAttributeCondition [%s]: Invalid attribute to test."), *GetNameSafe(Context.GetOwner()));
		return false;
	}

	// get the attribute set from the ASC
	const UAttributeSet* AttributeSet = ASC->GetAttributeSet(Attribute.GetAttributeSetClass());

	if (!AttributeSet)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGameplayAttributeCondition [%s]: Ability System does not have the attribute set to test."), *GetNameSafe(Context.GetOwner()));
		return false;
	}
	
	// get the attribute test value
	const float AttributeValue = Attribute.GetNumericValue(AttributeSet);
	
	bool bResult = false;
	
	switch (InstanceData.Operator)
	{
	case UE::StateTree::EComparisonOperator::Equal:
		bResult = FMath::IsNearlyEqual(AttributeValue, InstanceData.TestValue);
		break;
	case UE::StateTree::EComparisonOperator::NotEqual:
		bResult = !FMath::IsNearlyEqual(AttributeValue, InstanceData.TestValue);
		break;
	case UE::StateTree::EComparisonOperator::Less:
		bResult = AttributeValue < InstanceData.TestValue;
		break;
	case UE::StateTree::EComparisonOperator::LessOrEqual:
		bResult = AttributeValue <= InstanceData.TestValue;
		break;
	case UE::StateTree::EComparisonOperator::Greater:
		bResult = AttributeValue > InstanceData.TestValue;
		break;
	case UE::StateTree::EComparisonOperator::GreaterOrEqual:
		bResult = AttributeValue >= InstanceData.TestValue;
		break;
	default:
		ensureMsgf(false, TEXT("Unhandled operator %d"), InstanceData.Operator);
		return false;
	}

	return bInvert ? !bResult : bResult;
}

#if WITH_EDITOR

FText FStateTreeGameplayAttributeCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GameplayAttributeConditionInvalidTarget", "<b>WARNING: Invalid Target. Check the Target Actor Binding.</>");	
	}

	if (!Attribute.IsValid())
	{
		return LOCTEXT("GameplayAttributeConditionInvalidAttribute", "<b>WARNING: Invalid Attribute. Select an Attribute to test.</>");	
	}

	// get the attribute name
	FText AttributeText = FText::FromString(Attribute.GetName());

	// get the operator text
	FText OperatorText = UE::StateTree::DescHelpers::GetOperatorText(InstanceData->Operator, Formatting);

	// get the test value text
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 1;
	Options.MaximumFractionalDigits = 3;

	FText ValueText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TestValue)), Formatting);

	if (ValueText.IsEmpty())
	{
		ValueText = FText::AsNumber(InstanceData->TestValue, &Options);
	}

	// get the invert text
	FText InvertText = UE::StateTree::DescHelpers::GetInvertText(bInvert, Formatting);

	// format the description
	FText FormatText = FText::Format(LOCTEXT("GameplayAttributeConditionFormat", "{0}{1} on {2} {3} {4}"), InvertText, AttributeText, TargetText, OperatorText, ValueText);

	return FormatText;
}

EDataValidationResult FStateTreeGameplayAttributeCondition::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure we have a valid attribute assigned
	if (!Attribute.IsValid())
	{
		CompileContext.AddValidationError(LOCTEXT("GameplayAttributeConditionErrorMissingAttribute", "Invalid Attribute. Select an Attribute to test."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif

#undef LOCTEXT_NAMESPACE