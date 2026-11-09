// Copyright Epic Games, Inc. All Rights Reserved.

#include "StateTree/STGASPropertyFunctions.h"
#include "SampleLogging.h"
#include "AttributeSet.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#define LOCTEXT_NAMESPACE "StateTreeGASPropertyFunctions"

void FStateTreeGetAttributePropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// get the ASC
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor);

	if (!ASC)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGetAttributePropertyFunction [%s]: No Ability System Component present on Target Actor."), *GetNameSafe(Context.GetOwner()));

		InstanceData.Result = 0.0f;
		return;
	}

	// ensure the attribute is valid
	if (!Attribute.IsValid())
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGetAttributePropertyFunction [%s]: Invalid attribute to test."), *GetNameSafe(Context.GetOwner()));

		InstanceData.Result = 0.0f;
		return;
	}

	// get the attribute set from the ASC
	const UAttributeSet* AttributeSet = ASC->GetAttributeSet(Attribute.GetAttributeSetClass());

	if (!AttributeSet)
	{
		UE_LOG(LogSampleStateTree, Error, TEXT("FStateTreeGetAttributePropertyFunction [%s]: Ability System does not have the attribute set to test."), *GetNameSafe(Context.GetOwner()));

		InstanceData.Result = 0.0f;
		return;
	}
	
	// get the attribute test value and assign it to the result
	InstanceData.Result = Attribute.GetNumericValue(AttributeSet);
}

#if WITH_EDITOR

FText FStateTreeGetAttributePropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GameplayAttributePFInvalidTarget", "WARNING: Invalid Target. Check the Target Actor Binding.");	
	}

	// get the name of the attribute
	FText AttributeText;

	// get the name of the attribute
	if (!Attribute.IsValid())
	{
		return LOCTEXT("GameplayAttributePFInvalidAttribute", "WARNING: Invalid Attribute.Select an Attribute to get.");	
	}

	AttributeText = FText::FromString(Attribute.GetName());

	return FText::Format(LOCTEXT("GameplayAttributePFGetAttribute", "Get Value of Attribute {0} on {1}"), AttributeText, TargetText);
}

EDataValidationResult FStateTreeGetAttributePropertyFunction::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure we have a valid attribute assigned
	if (!Attribute.IsValid())
	{
		CompileContext.AddValidationError(LOCTEXT("GameplayAttributeConditionErrorMissingAttribute", "Invalid Attribute. Select an Attribute to get."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif

#undef LOCTEXT_NAMESPACE