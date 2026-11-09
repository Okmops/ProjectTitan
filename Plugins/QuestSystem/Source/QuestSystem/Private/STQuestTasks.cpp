// Copyright Epic Games, Inc. All Rights Reserved.


#include "STQuestTasks.h"
#include "StateTreeExecutionContext.h"
#include "QuestLogging.h"
#include "QuestComponent.h"
#include "QuestInterface.h"

#define LOCTEXT_NAMESPACE "StateTreeQuestTasks"

EStateTreeRunStatus FStateTreeStartQuestTag::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the quest tag is not valid
	if (!InstanceData.QuestTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeStartQuestTag: Quest Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeStartQuestTag: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeStartQuestTag: Target Actor does not have a Quest Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// attempt to start the quest, report success or failure depending on the result
	if(QuestManager->StartQuest(InstanceData.QuestTag, true))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeStartQuestTag::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("StartQuestTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
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
			return LOCTEXT("StartQuestTaskInvalidTag", "<b>WARNING: Invalid Quest Tag. Check the Quest Tag Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("StartQuestTaskTarget", "Start Quest <b>{0}</> on Actor <b>{1}</>"), QuestText, TargetText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeStartQuestTag::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the quest tag is valid
	if (!InstanceData.QuestTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("StartQuestTaskErrorInvalidTag", "Invalid Quest Tag. Check the Quest Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeFinishQuestTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the quest tag is not valid
	if (!InstanceData.QuestTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeFinishQuestTask: Quest Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeFinishQuestTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeFinishQuestTask: Target Actor does not have a Quest Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// attempt to finish the quest, report success or failure depending on the result
	if(QuestManager->FinishQuest(InstanceData.QuestTag, !bFailQuest))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeFinishQuestTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("FinishQuestTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
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
			return LOCTEXT("FinishQuestTaskInvalidTag", "<b>WARNING: Invalid Quest Tag. Check the Quest Tag Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	if (bFailQuest)
	{
		OutText.AppendLine(FText::Format(LOCTEXT("FinishQuestTaskFail", "Fail Quest <b>{0}</> on Actor <b>{1}</>"), QuestText, TargetText));
	}
	else
	{
		OutText.AppendLine(FText::Format(LOCTEXT("FinishQuestTaskComplete", "Complete Quest <b>{0}</> on Actor <b>{1}</>"), QuestText, TargetText));
	}

	return OutText.ToText();
}

EDataValidationResult FStateTreeFinishQuestTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the quest tag is valid
	if (!InstanceData.QuestTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("FinishQuestTaskErrorInvalidTag", "Invalid Quest Tag. Check the Quest Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeSendQuestUpdateTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the quest tag is not valid
	if (!InstanceData.QuestTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestUpdateTask: Quest Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the quest step tag is not valid
	if (!InstanceData.QuestStepTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestUpdateTask: Quest Step Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestUpdateTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestUpdateTask: Target Actor does not have a Quest Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// attempt to finish the quest, report success or failure depending on the result
	if(QuestManager->UpdateQuest(InstanceData.QuestTag, InstanceData.QuestStepTag, InstanceData.OptionalData))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeSendQuestUpdateTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("SendQuestUpdateTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
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
			return LOCTEXT("SendQuestUpdateTaskInvalidQuest", "<b>WARNING: Invalid Quest Tag. Check the Quest Tag Binding.</>");
		}
	}

	// get the name of the quest step tag binding
	FText StepText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestStepTag)), Formatting);
	if (StepText.IsEmpty())
	{
		// is the quest tag valid?
		if (InstanceData->QuestStepTag.IsValid())
		{
			// use the tag itself
			StepText = FText::FromString(InstanceData->QuestStepTag.ToString());
		}
		else
		{
			return LOCTEXT("SendQuestUpdateTaskInvalidStep", "<b>WARNING: Invalid Quest Step Tag. Check the Quest Step Tag Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("SendQuestUpdateTaskUpdate", "Send Update for Quest <b>{0}</> Step <b>{1}</> to Actor <b>{2}</>"), QuestText, StepText, TargetText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeSendQuestUpdateTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the quest tag is valid
	if (!InstanceData.QuestTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("SendQuestUpdateTaskErrorInvalidQuest", "Invalid Quest Tag. Check the Quest Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	// ensure the quest step tag is valid
	if (!InstanceData.QuestStepTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestStepTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("SendQuestUpdateTaskErrorInvalidStep", "Invalid Quest Step Tag. Check the Quest Step Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeQuestStatusTagTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the quest status tag is not valid
	if (!InstanceData.QuestStatusTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeQuestStatusTagTask: Quest Status Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeQuestStatusTagTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeQuestStatusTagTask: Target Actor does not have a Quest Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// do we want to add or remove the tag?
	if (bRemoveTag)
	{
		// attempt to remove the quest status tag, report success if it was removed
		if(QuestManager->RemoveQuestStatusTag(InstanceData.QuestStatusTag))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		// attempt to add the quest status tag, report success if it was added
		if(QuestManager->AddQuestStatusTag(InstanceData.QuestStatusTag))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeQuestStatusTagTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("QuestStatusTagTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// get the name of the quest status tag binding
	FText QuestText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestStatusTag)), Formatting);
	if (QuestText.IsEmpty())
	{
		// is the quest status tag valid?
		if (InstanceData->QuestStatusTag.IsValid())
		{
			// use the tag itself
			QuestText = FText::FromString(InstanceData->QuestStatusTag.ToString());
		}
		else
		{
			return LOCTEXT("QuestStatusTagTaskInvalidTag", "<b>WARNING: Invalid Status Quest Tag. Check the Quest Tag Binding.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	if (bRemoveTag)
	{
		OutText.AppendLine(FText::Format(LOCTEXT("QuestStatusTagTaskRemove", "Remove Quest Status <b>{0}</> on Actor <b>{1}</>"), QuestText, TargetText));
	}
	else
	{
		OutText.AppendLine(FText::Format(LOCTEXT("QuestStatusTagTaskAdd", "Add Quest Status <b>{0}</> to Actor <b>{1}</>"), QuestText, TargetText));
	}

	return OutText.ToText();
}

EDataValidationResult FStateTreeQuestStatusTagTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the quest tag is valid
	if (!InstanceData.QuestStatusTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestStatusTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("QuestStatusTagTaskErrorInvalidTag", "Invalid Quest Status Tag. Check the Quest Status Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeSendQuestEventTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// ignore if there's no Enter event
	if (!InstanceData.EnterEventTag.IsValid())
	{
		return EStateTreeRunStatus::Running;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestEventTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestEventTask: Target Actor does not have a Quest Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// send the Enter event
	QuestManager->SendQuestEvent(InstanceData.EnterEventTag, InstanceData.Payload);
	
	return EStateTreeRunStatus::Succeeded;
}

void FStateTreeSendQuestEventTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// ignore if there's no Exit event
	if (!InstanceData.ExitEventTag.IsValid())
	{
		return;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestEventTask: Target Actor is not valid."));
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestEventTask: Target Actor does not have a Quest Manager Component."));
		return;
	}

	// send the Enter event
	QuestManager->SendQuestEvent(InstanceData.ExitEventTag, InstanceData.Payload);
}

#if WITH_EDITOR

FText FStateTreeSendQuestEventTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("SendQuestEventTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// keep track of Enter and Exit events.
	bool bHasEnterEvent = true;
	bool bHasExitEvent = true;

	// get the name of the enter event tag binding
	FText EnterText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, EnterEventTag)), Formatting);
	if (EnterText.IsEmpty())
	{
		// is the enter event tag valid?
		if (InstanceData->EnterEventTag.IsValid())
		{
			// use the tag itself
			EnterText = FText::FromString(InstanceData->EnterEventTag.ToString());
		}
		else
		{
			// we have no valid Enter event.
			bHasEnterEvent = false;
		}
	}

	// get the name of the Exit event tag binding
	FText ExitText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, ExitEventTag)), Formatting);
	if (ExitText.IsEmpty())
	{
		// is the Exit event tag valid?
		if (InstanceData->ExitEventTag.IsValid())
		{
			// use the tag itself
			ExitText = FText::FromString(InstanceData->ExitEventTag.ToString());
		}
		else
		{
			// we have no valid Exit event.
			bHasExitEvent = false;
		}
	}

	if (!bHasEnterEvent && !bHasExitEvent)
	{
		return LOCTEXT("SendQuestEventTaskInvalidEvents", "<b>WARNING: No valid Enter or Exit events. Make sure at least one of them is valid.</>");
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("SendQuestEventTaskTarget", "Send Quest Events to Actor <b>{0}</>"), TargetText));

	if (bHasEnterEvent)
	{
		OutText.AppendLine(FText::Format(LOCTEXT("SendQuestEventTaskEnterEvent", "<s>On Enter: {0}</>"), EnterText));
	}

	if (bHasExitEvent)
	{
		OutText.AppendLine(FText::Format(LOCTEXT("SendQuestEventTaskExitEvent", "<s>On Exit: {0}</>"), ExitText));
	}

	return OutText.ToText();
}

EDataValidationResult FStateTreeSendQuestEventTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// check if we have valid or bound enter or exit event tags
	const bool bHasEnterEvent = InstanceData.EnterEventTag.IsValid() || CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, EnterEventTag));
	const bool bHasExitEvent = InstanceData.ExitEventTag.IsValid() || CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, ExitEventTag));

	// ensure at least one event is valid
	if (!(bHasEnterEvent || bHasExitEvent))
	{
		CompileContext.AddValidationError(LOCTEXT("SendQuestEventTaskErrorInvalidEvents", "No valid Enter or Exit events. Make sure at least one of them is valid."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeChangeQuestQuantityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the tag is invalid
	if (!InstanceData.QuestQuantityTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeChangeQuestQuantityTask: Quest Quantity Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeChangeQuestQuantityTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the count delta is zero
	if (InstanceData.CountDelta == 0)
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeChangeQuestQuantityTask: Count Delta is zero."));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeSendQuestEventTask: Target Actor does not have a Quest Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// update the quantity
	QuestManager->AddQuestQuantity(InstanceData.QuestQuantityTag, InstanceData.CountDelta);
	
	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR

FText FStateTreeChangeQuestQuantityTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("ChangeQuestQuantityTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// get the name of the tag binding
	FText QuestText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestQuantityTag)), Formatting);
	if (QuestText.IsEmpty())
	{
		// is the quest status tag valid?
		if (InstanceData->QuestQuantityTag.IsValid())
		{
			// use the tag itself
			QuestText = FText::FromString(InstanceData->QuestQuantityTag.ToString());
		}
		else
		{
			return LOCTEXT("ChangeQuestQuantityTaskInvalidTag", "<b>WARNING: Invalid Quest Quantity Tag. Check the Quest Quantity Binding.</>");
		}
	}

	// get the name of the count binding
	FText CountText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, CountDelta)), Formatting);
	if (CountText.IsEmpty())
	{
		// is the count valid?
		if (InstanceData->CountDelta != 0)
		{
			// use the tag itself
			CountText = CountText = FText::AsNumber(InstanceData->CountDelta);
		}
		else
		{
			return LOCTEXT("ChangeQuestQuantityTaskInvalidCount", "<b>WARNING: Invalid Count Delta. Ensure the Count Delta is not zero.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("ChangeQuestQuantityTaskTarget", "Change Quest Quantity <b>{0}</> on Actor <b>{1}</>"), QuestText, TargetText));
	OutText.AppendLine(FText::Format(LOCTEXT("ChangeQuestQuantityTaskCount", "<s>Count Delta: {0}</>"), CountText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeChangeQuestQuantityTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the quest tag is valid
	if (!InstanceData.QuestQuantityTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestQuantityTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("ChangeQuestQuantityTaskInvalidTag", "Invalid Quest Quantity Tag. Check the Quest Quantity Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	if (InstanceData.CountDelta == 0 && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, CountDelta)))
	{
		CompileContext.AddValidationError(LOCTEXT("ChangeQuestQuantityTaskInvalidCount", "Invalid Count Delta. Ensure the Count Delta is not zero."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeWaitQuestQuantityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// fail if the tag is invalid
	if (!InstanceData.QuestQuantityTag.IsValid())
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeWaitQuestQuantityTask: Quest Quantity Tag is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target actor is not valid
	if (!IsValid(InstanceData.TargetActor))
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeWaitQuestQuantityTask: Target Actor is not valid."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the target value is less than 1
	if (InstanceData.TargetValue < 1)
	{
		UE_LOG(LogQuest, Error, TEXT("FStateTreeWaitQuestQuantityTask: Target Value is less than 1"));
		return EStateTreeRunStatus::Failed;
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
		UE_LOG(LogQuest, Error, TEXT("FStateTreeWaitQuestQuantityTask: Target Actor does not have a Quest Manager Component."));
		return EStateTreeRunStatus::Failed;
	}

	// compare the quantity to the target value
	return (QuestManager->GetQuestQuantity(InstanceData.QuestQuantityTag) >= InstanceData.TargetValue) ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR

FText FStateTreeWaitQuestQuantityTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the target actor binding
	FText TargetText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetActor)), Formatting);
	if (TargetText.IsEmpty())
	{
		return LOCTEXT("WaitQuestQuantityTaskInvalidTarget", "<b>WARNING: Invalid Target Actor. Check the Target Actor Binding.</>");
	}

	// get the name of the tag binding
	FText QuestText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestQuantityTag)), Formatting);
	if (QuestText.IsEmpty())
	{
		// is the quest status tag valid?
		if (InstanceData->QuestQuantityTag.IsValid())
		{
			// use the tag itself
			QuestText = FText::FromString(InstanceData->QuestQuantityTag.ToString());
		}
		else
		{
			return LOCTEXT("WaitQuestQuantityTaskInvalidTag", "<b>WARNING: Invalid Quest Quantity Tag. Check the Quest Quantity Binding.</>");
		}
	}

	// get the name of the target value binding
	FText CountText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetValue)), Formatting);
	if (CountText.IsEmpty())
	{
		// is the target value valid?
		if (InstanceData->TargetValue > 0)
		{
			// use the tag itself
			CountText = CountText = FText::AsNumber(InstanceData->TargetValue);
		}
		else
		{
			return LOCTEXT("WaitQuestQuantityTaskInvalidValue", "<b>WARNING: Invalid Target Value. Ensure the Target Value is greater than zero.</>");
		}
	}

	// format and output the description
	FTextBuilder OutText;

	OutText.AppendLine(FText::Format(LOCTEXT("WaitQuestQuantityTaskTarget", "Quest Quantity <b>{0}</> on Actor <b>{1}</> is at least <b>{2}</>"), QuestText, TargetText, CountText));

	return OutText.ToText();
}

EDataValidationResult FStateTreeWaitQuestQuantityTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the quest tag is valid
	if (!InstanceData.QuestQuantityTag.IsValid() && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, QuestQuantityTag)))
	{
		CompileContext.AddValidationError(LOCTEXT("WaitQuestQuantityTaskInvalidTag", "Invalid Quest Quantity Tag. Check the Quest Quantity Tag Binding."));
		Result = EDataValidationResult::Invalid;
	}

	if (InstanceData.TargetValue <= 0 && !CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetValue)))
	{
		CompileContext.AddValidationError(LOCTEXT("WaitQuestQuantityTaskInvalidCount", "Invalid Target Value. Ensure the Target Value is greater than zero."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE