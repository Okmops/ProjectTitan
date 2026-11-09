// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogueNode.h"
#include "CommonActivatableWidget.h"
#include "Input/UIActionBindingHandle.h"
#include "DialogueWindow.generated.h"

class UStateTreeComponent;
class UInputAction;

DECLARE_DELEGATE_TwoParams(FOnDialogueResponse, const FGameplayTag&, const FDialogueResponsePayload& Payload);

/**
 *  UDialogueWindow
 *  Displays a dialogue tree node interaction consisting of one or more text pages and a set of user responses.
 */
UCLASS(abstract)
class STATETREEDIALOGUE_API UDialogueWindow : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:

	/** Delegate to call when the dialogue reaches a response */
	FOnDialogueResponse OnDialogueResponse;

protected:

	/** Dialogue node used to populate this window. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FDialogueNode DialogueNode;

	/** Current text page index on the dialogue node */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	int32 CurrentPage = 0;

	/** Optional data payload included with the dialogue response */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FDialogueResponsePayload ResponsePayload;

	/** Input Action to use to cancel this dialogue */
	UPROPERTY(EditAnywhere, Category ="Input")
	TObjectPtr <UInputAction> CancelDialogueAction;

	/** Cancel dialogue binding handle */
	FUIActionBindingHandle CancelBindingHandle;

	FFormatNamedArguments DialogueNodeArguments;

protected:

	/** Construct override to register the cancel action binding */
	virtual void NativeConstruct() override;

	/** Destruct override to deregister the cancel action binding */
	virtual void NativeDestruct() override;

public:

	/** Sets the dialogue node for this window */
	UFUNCTION(BlueprintCallable, Category= "Dialogue")
	void SetDialogueNode(const FDialogueNode& Node);

	/** Called from response buttons to trigger a dialogue node response */
	UFUNCTION(BlueprintCallable, Category= "Dialogue")
	void SendResponse(const FGameplayTag& ResponseTag);

	/** Advances to the next page in the dialogue node, or completes the dialogue if it's the last page */
	UFUNCTION(BlueprintCallable, Category= "Dialogue")
	void NextPage();

	/** Returns to the previous page in the dialogue node */
	UFUNCTION(BlueprintCallable, Category= "Dialogue")
	void PreviousPage();

	/** Cancels the dialogue window if the current node is cancellable */
	UFUNCTION(BlueprintCallable, Category= "Dialogue")
	void CancelDialogue();

	/** Closes the current window */
	UFUNCTION(BlueprintCallable, Category= "Dialogue")
	void CloseWindow();

protected:

	/** Helper function to apply all passed node data to the text */
	UFUNCTION(BlueprintCallable, Category= "Dialogue")
	FText FormatTextWithDialogueData(const FText& InputText) const;
	
	/** Updates the widget with new node data */
	void Native_OnDialogueNodeUpdated();

	/** Constructs sub-widgets in response to a dialogue node data change */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "On Dialogue Constructed"))
	void BP_OnDialogueConstructed();

	/** Allows sub-widgets to update themselves when dialogue node data changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "On Dialogue Updated"))
	void BP_OnDialogueUpdated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue", meta = (DisplayName = "On Dialogue Closed"))
	void BP_OnWindowClosed();

};
