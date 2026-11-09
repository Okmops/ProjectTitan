// Copyright Epic Games, Inc. All Rights Reserved.


#include "StateTreeDialogueTasks.h"
#include "DialogueWindow.h"
#include "DialogueSystemLogging.h"
#include "StateTreeExecutionContext.h"
#include "Blueprint/UserWidget.h"
#include "StateTreeAsyncExecutionContext.h"


#define LOCTEXT_NAMESPACE "DialogueTasks"

EStateTreeRunStatus FStateTreeUpdateDialogueTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.DialogueWindow)
	{
		UE_LOG(LogDialogueSystem, Error, TEXT("FStateTreeUpdateDialogueTask: Invalid Dialogue Window."));
		return EStateTreeRunStatus::Failed;
	}

	// fail if the dialogue node has no pages to display
	if (InstanceData.DialogueNode.Pages.Num() < 1)
	{
		UE_LOG(LogDialogueSystem, Error, TEXT("FStateTreeUpdateDialogueTask: Attempted to open a dialogue node with zero pages."));
		return EStateTreeRunStatus::Failed;
	}

	// bind to the user response delegate
	InstanceData.DialogueWindow->OnDialogueResponse.BindLambda(
		[WeakContext = Context.MakeWeakExecutionContext()](const FGameplayTag& ResponseTag, const FDialogueResponsePayload& ResponsePayload)
		{
			// has this dialogue been completed?
			if (ResponseTag.MatchesTagExact(TAG_Dialogue_DialogueComplete))
			{
				// succeed the task
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
				return;
			}
			// has this dialogue been cancelled?
			else if (ResponseTag.MatchesTagExact(TAG_Dialogue_DialogueCancelled))
			{
				// fail the task
				WeakContext.FinishTask(EStateTreeFinishTaskType::Failed);
				return;
			}

			// send the response event to the StateTree
			WeakContext.SendEvent(ResponseTag, FInstancedStruct::Make(ResponsePayload));
		}
	);

	// initialize the window
	InstanceData.DialogueWindow->SetDialogueNode(InstanceData.DialogueNode);

	// activate the widget
	InstanceData.DialogueWindow->ActivateWidget();

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR

FText FStateTreeUpdateDialogueTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	FTextBuilder OutText;

	// add the first line
	OutText.AppendLine(LOCTEXT("UpdateDialogueTaskUpdate", "Update Dialogue Window"));

	// get the dialogue node binding
	FText NodeText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, DialogueNode)), Formatting);
	if (NodeText.IsEmpty())
	{
		// report an error if there's not at least one page of dialogue to display
		if (InstanceData->DialogueNode.Pages.Num() < 1)
		{
			return LOCTEXT("UpdateDialogueTaskInvalidNode", "<b>WARNING: Dialogue Node has no pages to display. Add at least one page.</>");
		}

		// if there's no binding, output the number of pages and responses
		OutText.AppendLine(FText::Format(LOCTEXT("UpdateDialogueTaskPages", "<s>Text Pages: </> {0}"), FText::AsNumber(InstanceData->DialogueNode.Pages.Num())));
		OutText.AppendLine(FText::Format(LOCTEXT("UpdateDialogueTaskResponses", "<s>Responses: </> {0}"), FText::AsNumber(InstanceData->DialogueNode.Responses.Num())));

	}
	else
	{

		// otherwise, output the binding name
		OutText.AppendLine(FText::Format(LOCTEXT("UpdateDialogueTaskNode", "<s>Dialogue Node: </> {0}"), NodeText));

	}

	// cancellable
	OutText.AppendLine(InstanceData->DialogueNode.bIsCancellable ? LOCTEXT("UpdateDialogueTaskCancellable", "<s>Dialogue can be Cancelled</>") : LOCTEXT("OpenDialogueTaskNoCancelable", "<s>Dialogue cannot be Cancelled</>"));

	return OutText.ToText();
}

EDataValidationResult FStateTreeUpdateDialogueTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the dialogue node has at least one page if it's not bound
	if (!CompileContext.HasBindingForProperty(GET_MEMBER_NAME_CHECKED(FInstanceDataType, DialogueNode)) && InstanceData.DialogueNode.Pages.Num() < 1)
	{
		CompileContext.AddValidationError(LOCTEXT("UpdateDialogueTaskErrorNoPages", "Dialogue Node has no pages to display. Add at least one page."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

EStateTreeRunStatus FStateTreeCreateDialogueTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// skip if the PC is not valid
	if (!IsValid(InstanceData.InteractingPlayer))
	{
		UE_LOG(LogDialogueSystem, Error, TEXT("FStateTreeCreateDialogueTask: Attempted to create a window with no valid interacting PlayerController."));
		return EStateTreeRunStatus::Failed;
	}

	// skip if the window class is not valid
	if (!DialogueWindowClass)
	{
		UE_LOG(LogDialogueSystem, Error, TEXT("FStateTreeCreateDialogueTask: Attempted to create an invalid window class."));
		return EStateTreeRunStatus::Failed;
	}

	// create the widget
	InstanceData.DialogueWindow = CreateWidget<UDialogueWindow>(InstanceData.InteractingPlayer, DialogueWindowClass);

	if (!InstanceData.DialogueWindow)
	{
		return EStateTreeRunStatus::Failed;
	}

	// display the dialogue window
	InstanceData.DialogueWindow->AddToPlayerScreen();

	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR

FText FStateTreeCreateDialogueTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the interacting player
	FText PlayerText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, InteractingPlayer)), Formatting);
	if (PlayerText.IsEmpty())
	{
		return LOCTEXT("CreateDialogueTaskInvalidOwner", "<b>WARNING: No interacting player binding set. Check the Interacting Player Binding.</>");
	}

	// get the name of the window class
	FText ClassText; 

	if (DialogueWindowClass)
	{
		ClassText = DialogueWindowClass->GetDisplayNameText();
	}
	else
	{
		return LOCTEXT("CreateDialogueTaskInvalidClass", "<b>WARNING: Invalid Dialogue Window Class. Select a window class to create.</>");
	}
	
	return FText::Format(LOCTEXT("CreateDialogueTaskCreate", "Create Dialogue Window of Class <b>{0}</> for Player <b>{1}</>"), ClassText, PlayerText);
}

EDataValidationResult FStateTreeCreateDialogueTask::Compile(UE::StateTree::ICompileNodeContext& CompileContext)
{
	const FInstanceDataType& InstanceData = CompileContext.GetInstanceDataView().Get<FInstanceDataType>();

	EDataValidationResult Result = EDataValidationResult::Valid;

	// ensure the window class is valid
	if (!DialogueWindowClass)
	{
		CompileContext.AddValidationError(LOCTEXT("CreateDialogueTaskErrorInvalidClass", "Invalid Dialogue Window Class. Select a window class to create."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif // WITH_EDITOR

void FStateTreeCloseDialogueTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// if we have a valid window, close it
	if (InstanceData.DialogueWindow)
	{
		InstanceData.DialogueWindow->CloseWindow();
		InstanceData.DialogueWindow = nullptr;
	}
}

#if WITH_EDITOR

FText FStateTreeCloseDialogueTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	// get the instance data
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	// get the name of the interacting player
	FText WindowText = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, DialogueWindow)), Formatting);
	if (WindowText.IsEmpty())
	{
		return LOCTEXT("CloseDialogueTaskInvalidWindow", "<b>WARNING: No Dialogue Window binding set. Check the Dialogue Window Binding.</>");
	}
	
	return FText::Format(LOCTEXT("CloseDialogueTaskClose", "Close Dialogue Window <b>{0}</> on State Exit"), WindowText);
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE