// Copyright Epic Games, Inc. All Rights Reserved.


#include "DialogueButton.h"
#include "DialogueWindow.h"
#include "DialogueSystemLogging.h"

void UDialogueResponseButton::HandleButtonClicked()
{
	Super::HandleButtonClicked();

	// tell the parent window to send the response
	if (ParentWindow)
	{
		ParentWindow->SendResponse(ResponseTag);
	}
	else
	{
		UE_LOG(LogDialogueSystem, Error, TEXT("UDialogueResponseButton: No valid parent window, skipping response."));
	}
}

void UDialogueResponseButton::InitializeButton(UDialogueWindow* Window, const FGameplayTag& Response)
{
	// ensure the parent window is valid
	if (!IsValid(Window))
	{
		UE_LOG(LogDialogueSystem, Error, TEXT("UDialogueResponseButton: No valid parent window, the button will not work as intended."));

		return;
	}

	// set the parent window
	ParentWindow = Window;

	// save the response tag
	ResponseTag = Response;
}
