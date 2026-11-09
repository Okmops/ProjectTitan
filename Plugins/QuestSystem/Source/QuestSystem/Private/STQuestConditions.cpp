// Copyright Epic Games, Inc. All Rights Reserved.


#include "STQuestConditions.h"
#include "StateTreeExecutionContext.h"
#include "QuestLogging.h"
#include "QuestComponent.h"
#include "QuestInterface.h"

#define LOCTEXT_NAMESPACE "StateTreeQuestConditions"

bool FQuestCompletionStatusCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the quest tag is not valid
	if (!InstanceData.QuestTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FQuestCompletionStatusCondition: Quest Tag is not valid."));
		return false;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FQuestCompletionStatusCondition: Target Actor is not valid."));
		return false;
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
		UE_LOG(LogQuest, Error, TEXT("FQuestCompletionStatusCondition: Target Actor does not have a Quest Manager Component."));
		return false;
	}

	bool bCondition = false;

	// return the completion status of the quest
	switch(QuestStatusCheck)
	{
		case EQuestStatusCheck::Active:
			bCondition = QuestManager->IsQuestActive(InstanceData.QuestTag);
			break;
		case EQuestStatusCheck::Completed:
			bCondition = QuestManager->IsQuestCompleted(InstanceData.QuestTag);
			break;
		case EQuestStatusCheck::Failed:
			bCondition = QuestManager->IsQuestFailed(InstanceData.QuestTag);
			break;
	}

	return bInvert? !bCondition : bCondition;
}

#if WITH_EDITOR

FText FQuestCompletionStatusCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("QuestCompletionConditionInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// get the name of the quest tag binding
	FText QuestText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestTag)), Formatting);
	if (QuestText.IsEmpty())
	{
		// is the quest tag valid?
		if (InstanceData->QuestTag.IsValid())
		{
			// use the tag itself
			QuestText = FText::FromString(InstanceData->QuestTag.ToString());
		}
		else
		{
			return LOCTEXT("QuestCompletionConditionInvalidTag", "<b>WARNING: Invalid Quest Tag. Check the Quest Tag Binding.</>");
		}
	}

	FText InvertText = bInvert ? LOCTEXT("QuestCompletionConditionInverted", "<b>IS NOT</>") : LOCTEXT("QuestCompletionConditionNotInverted", "<b>IS</>");

	FText StatusText;

	switch(QuestStatusCheck)
	{
		case EQuestStatusCheck::Active:
			StatusText = LOCTEXT("QuestCompletionConditionActive", "Active");
			break;
		case EQuestStatusCheck::Completed:
			StatusText = LOCTEXT("QuestCompletionConditionCompleted", "Completed");
			break;
		case EQuestStatusCheck::Failed:
			StatusText = LOCTEXT("QuestCompletionConditionFailed", "Failed");
			break;
	}

	// format and output the description
	return FText::Format(LOCTEXT("QuestCompletionConditionDesc", "Quest {0} {1} {2} on Actor {3}"), QuestText, InvertText, StatusText, TargetText);
}

EDataValidationResult FQuestCompletionStatusCondition::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the item tag is valid
	if (!InstanceData.QuestTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("QuestCompletionStatusConditionErrorInvalidTag", "Invalid Quest Tag. Check the Quest Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

bool FQuestStatusQueryCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FQuestStatusQueryCondition: Target Actor is not valid."));
		return false;
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
		UE_LOG(LogQuest, Error, TEXT("FQuestStatusQueryCondition: Target Actor does not have a Quest Manager Component."));
		return false;
	}

	// return the result of the query
	return QuestManager->DoQuestStatusQuery(QuestStatusQuery);
}

#if WITH_EDITOR

FText FQuestStatusQueryCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("QuestStatusQueryConditionInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// is the query valid?
	if (QuestStatusQuery.IsEmpty())
	{
		return LOCTEXT("QuestStatusQueryConditionInvalidQuery", "<b>WARNING: Invalid Gameplay Tag Query. Add at least one test to the query.</>");
	}

	FText QueryText = FText::Format(LOCTEXT("QuestStatusQueryConditionQuery", "Query: <s>{0}</>"), FText::FromString(QuestStatusQuery.GetDescription()));

	// format and output the description
	return FText::Format(LOCTEXT("QuestStatusQueryConditionDesc", "Do Gameplay Tag Query on Quest Data for Actor <b>{0}</> \n {1}"), TargetText, QueryText);
}

EDataValidationResult FQuestStatusQueryCondition::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the query is valid
	if (QuestStatusQuery.IsEmpty())
	{
		CompileContext.AddValidationError(LOCTEXT("QuestStatusQueryConditionErrorInvalidQuery", "Invalid Gameplay Tag Query. Add at least one test to the query."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE