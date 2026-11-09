// Copyright Epic Games, Inc. All Rights Reserved.


#include "STQuestPropertyFunctions.h"
#include "StateTreeExecutionContext.h"
#include "QuestLogging.h"
#include "QuestComponent.h"
#include "QuestInterface.h"

#define LOCTEXT_NAMESPACE "StateTreeQuestPropertyFunctions"

void FStateTreeHasQuestTagPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// assume false by default
	InstanceData.bResult = false;

	// fail if the quest tag is not valid
	if (!InstanceData.QuestStatusTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeHasQuestTagPropertyFunction: Quest Status Tag is not valid."));
		return;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeHasQuestTagPropertyFunction: Target Actor is not valid."));
		return;
	}

	// get the quest manager comp
	UQuestManagerComponent* QuestManager = nullptr;

	// first try to get it through the quest interface
	if (IQuestInterface* QuestInterface = Cast<IQuestInterface>(InstanceData.TargetActor))
	{
		QuestManager = QuestInterface->GetQuestManagerComponent();
	}

	// if we fail, search the actor for an quest manager comp
	if (!QuestManager)
	{
		QuestManager = InstanceData.TargetActor->FindComponentByClass<UQuestManagerComponent>();
	}		

	if (!QuestManager)
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeHasQuestTagPropertyFunction: Target Actor does not have a Quest Manager Component."));
		return;
	}

	// set the result from the Quest Manager Comp
	InstanceData.bResult = QuestManager->HasQuestStatusTag(InstanceData.QuestStatusTag);
}

#if WITH_EDITOR

FText FStateTreeHasQuestTagPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("HasQuestTagPropertyFunctionInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// get the name of the quest tag binding
	FText QuestText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestStatusTag)), Formatting);
	if (QuestText.IsEmpty())
	{
		// is the quest tag valid?
		if (InstanceData->QuestStatusTag.IsValid())
		{
			// use the tag itself
			QuestText = FText::FromString(InstanceData->QuestStatusTag.ToString());
		}
		else
		{
			return LOCTEXT("HasQuestTagPropertyFunctionInvalidTag", "<b>WARNING: Invalid Quest Tag. Check the Quest Tag Binding.</>");
		}
	}

	// format and return the output
	return FText::Format(LOCTEXT("HasQuestTagPropertyFunctionTarget", "Actor {0} has Quest Status Tag {1}"), TargetText, QuestText);
}

#endif //WITH_EDITOR

void FStateTreeGetQuestQuantityPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// assume 0 by default
	InstanceData.Result = 0;

	// fail if the quest tag is not valid
	if (!InstanceData.QuestQuantityTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeGetQuestQuantityPropertyFunction: Quest Quantity Tag is not valid."));
		return;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeGetQuestQuantityPropertyFunction: Target Actor is not valid."));
		return;
	}

	// get the quest manager comp
	UQuestManagerComponent* QuestManager = nullptr;

	// first try to get it through the quest interface
	if (IQuestInterface* QuestInterface = Cast<IQuestInterface>(InstanceData.TargetActor))
	{
		QuestManager = QuestInterface->GetQuestManagerComponent();
	}

	// if we fail, search the actor for an quest manager comp
	if (!QuestManager)
	{
		QuestManager = InstanceData.TargetActor->FindComponentByClass<UQuestManagerComponent>();
	}		

	if (!QuestManager)
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeGetQuestQuantityPropertyFunction: Target Actor does not have a Quest Manager Component."));
		return;
	}

	// set the result from the Quest Manager Comp
	InstanceData.Result = QuestManager->GetQuestQuantity(InstanceData.QuestQuantityTag);
}

#if WITH_EDITOR

FText FStateTreeGetQuestQuantityPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("GetQuestQuantityPropertyFunctionInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// get the name of the tag binding
	FText QuestText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestQuantityTag)), Formatting);
	if (QuestText.IsEmpty())
	{
		// is the tag valid?
		if (InstanceData->QuestQuantityTag.IsValid())
		{
			// use the tag itself
			QuestText = FText::FromString(InstanceData->QuestQuantityTag.ToString());
		}
		else
		{
			return LOCTEXT("GetQuestQuantityPropertyFunctionInvalidTag", "<b>WARNING: Invalid Quest Tag. Check the Quest Tag Binding.</>");
		}
	}

	// format and return the output
	return FText::Format(LOCTEXT("GetQuestQuantityPropertyFunctionTarget", "Get Quest Quantity {0} from Actor {1}"), QuestText, TargetText);
}

#endif //WITH_EDITOR

#undef LOCTEXT_NAMESPACE