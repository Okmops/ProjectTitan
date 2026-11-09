// Copyright Epic Games, Inc. All Rights Reserved.


#include "DialogueWindow.h"
#include "DialogueNode.h"
#include "InputAction.h"
#include "CommonUITypes.h"
#include "Input/CommonUIInputTypes.h"
#include "DialogueSystemLogging.h"

void UDialogueWindow::NativeConstruct()
{
	if (CommonUI::IsEnhancedInputSupportEnabled())
	{
		// bind the cancel action
		if (CancelDialogueAction != nullptr)
		{
			FBindUIActionArgs BindArgs(CancelDialogueAction, false, FSimpleDelegate::CreateUObject(this, &UDialogueWindow::CancelDialogue));
			BindArgs.bIsPersistent = false;
			BindArgs.InputMode = ECommonInputMode::Menu;

			CancelBindingHandle = RegisterUIActionBinding(BindArgs);
		}
	}

	Super::NativeConstruct();
}

void UDialogueWindow::NativeDestruct()
{
	// unregister the cancel binding handle
	if (CancelBindingHandle.IsValid())
	{
		CancelBindingHandle.Unregister();
	}

	Super::NativeDestruct();
}

void UDialogueWindow::SetDialogueNode(const FDialogueNode& Node)
{
	DialogueNode = Node;

	// compile the node data into format named arguments
	DialogueNodeArguments.Empty(DialogueNode.FloatValues.Num() + DialogueNode.IntValues.Num() + DialogueNode.TextValues.Num());

	// save all float values as f0 - fn
	for (int32 i = 0; i < DialogueNode.FloatValues.Num(); ++i)
	{
		FString KeyString = FString::Printf(TEXT("f%d"), i);

		DialogueNodeArguments.Add(*KeyString, DialogueNode.FloatValues[i]);
	}

	// save all int values as i0 - in
	for (int32 i = 0; i < DialogueNode.IntValues.Num(); ++i)
	{
		FString KeyString = FString::Printf(TEXT("i%d"), i);

		DialogueNodeArguments.Add(*KeyString, DialogueNode.IntValues[i]);
	}

	// save all text values as t0 - tn
	for (int32 i = 0; i < DialogueNode.TextValues.Num(); ++i)
	{
		FString KeyString = FString::Printf(TEXT("t%d"), i);

		DialogueNodeArguments.Add(*KeyString, DialogueNode.TextValues[i]);
	}

	// ensure the node has at least one page before updating
	if (DialogueNode.Pages.Num() < 1)
	{
		UE_LOG(LogDialogueSystem, Error, TEXT("UDialogueWindow: Assigned dialogue node with zero pages, skipping window update."));
		return;
	}

	// update the window with the new node data
	Native_OnDialogueNodeUpdated();
}

void UDialogueWindow::SendResponse(const FGameplayTag& ResponseTag)
{
	// broadcast the dialogue response
	OnDialogueResponse.ExecuteIfBound(ResponseTag, ResponsePayload);
}

void UDialogueWindow::NextPage()
{
	// ignore if we have no pages to display
	if (DialogueNode.Pages.Num() < 1)
	{
		return;
	}

	// are we at the last page and not waiting on explicit responses?
	if (CurrentPage == DialogueNode.Pages.Num() - 1 && DialogueNode.Responses.Num() < 1)
	{
		// complete the dialogue
		SendResponse(TAG_Dialogue_DialogueComplete);
		return;	
	}

	// temporarily cache the previous page
	int32 Prev = CurrentPage;

	// increment the page, ensuring we don't go beyond the last
	CurrentPage = FMath::Clamp(CurrentPage + 1, 0, DialogueNode.Pages.Num() - 1);

	// has the page actually changed?
	if (CurrentPage != Prev)
	{
		// update the window
		BP_OnDialogueUpdated();
	}
}

void UDialogueWindow::PreviousPage()
{
	// ignore if we have no pages to display
	if (DialogueNode.Pages.Num() < 1)
	{
		return;
	}

	// temporarily cache the previous page
	int32 Prev = CurrentPage;

	// decrease the page, ensuring we don't go below the first page
	CurrentPage = FMath::Clamp(CurrentPage - 1, 0, DialogueNode.Pages.Num() - 1);

	// has the page actually changed?
	if (CurrentPage != Prev)
	{
		// update the window
		BP_OnDialogueUpdated();
	}
}

void UDialogueWindow::CancelDialogue()
{
	// ensure we can cancel this dialogue
	if (DialogueNode.bIsCancellable)
	{
		// send the cancel message
		SendResponse(TAG_Dialogue_DialogueCancelled);
	}
}

void UDialogueWindow::CloseWindow()
{
	// call the BP handler to animate the close
	BP_OnWindowClosed();
}

FText UDialogueWindow::FormatTextWithDialogueData(const FText& InputText) const
{
	// do we have any compiled arguments from the dialogue node?
	if (DialogueNodeArguments.Num() > 0)
	{
		// format the arguments
		return FText::Format(InputText, DialogueNodeArguments);
	}

	// just return the original text
	return InputText;
}

void UDialogueWindow::Native_OnDialogueNodeUpdated()
{
	// reset the page
	CurrentPage = 0;

	// call the BP handler to construct sub-widgets
	BP_OnDialogueConstructed();

	// call the BP handler to update sub-widgets
	BP_OnDialogueUpdated();
}